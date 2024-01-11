#include "Pop3Connection.hpp"

int POP3Connection::DefaultPort(bool SSL) {
    if (SSL) {
        return DEFAULT_SSL_PORT;
    }
    else {
        return DEFAULT_PORT;
    }
}

POP3Connection::POP3Connection() {
    _nameOfConnection = "POP3 Connection";
    setPort(std::to_string(DefaultPort(_isSSLEnabled)));
}

POP3Connection::~POP3Connection() {
    closeConnection();
}

void POP3Connection::setUser(const std::string& user) {
    _user = user;
}

void POP3Connection::setPass(const std::string& pass) {
    _pass = pass;
}

void POP3Connection::connectToServer() {
    if (_state != DISCONNECTED) {
        return;
    }
    connectSocket();
    _state = AUTHORIZATION;
    readLineResponse();

    log("Successfully connected to server");
}

void POP3Connection::login() {
    if (_state == TRANSACTION) {
        log("Warning: already logged in!");
        return;
    }
    if (_state != AUTHORIZATION) {
        throw ConnectException("Unable to log in -- not connected to server");
    }
    execCommand("USER " + _user);
    execCommand("PASS " + _pass);
    _state = TRANSACTION;

    _noopThread = std::thread(&POP3Connection::keepAlive, this);
    _threadStarted = true;

    log("Successfully logged in, notified no-op thread to start");
}

void POP3Connection::login(const std::string& user, const std::string& pass) {
    if (_state == TRANSACTION) {
        log("Warning: already logged in!");
        return;
    }
    _user = user;
    _pass = pass;
    login();
}

void POP3Connection::sendCommand(const std::string& command) {
    std::string tmpCommand = command + "\r\n";  //CRLF ending
    checkNoopThreadError();
    int status = writeToSocket(tmpCommand.c_str(), tmpCommand.length());
    if (status < 0) {
        throw ConnectException("Error writing command to server");
    }
}

std::string POP3Connection::execCommand(const std::string& command, bool expectsMultiline, SingleLineMessage processing) {
    checkNoopThreadError();
    std::lock_guard<std::mutex> lock(_commandMutex);
    sendCommand(command);
    if (!expectsMultiline) {
        auto response = readLineResponse(processing);
        assertResponse(response);
        return response;
    }
    else {
        auto response = readMultiLineResponse();
        assertResponse(response);
        return response;
    }
}

std::string POP3Connection::readLineResponse(bool raw) {
    std::string finalResult = "";
    bool error = false;

    char buffer[2] = {0, 0};
    while (not (buffer[0] == '\r' and buffer[1] == '\n')) {
        char singleCharBuffer;

        checkNoopThreadError();
        int readCode = readFromSocket(&singleCharBuffer, sizeof(char));
        if (readCode <= 0) {
            throw ConnectException("Could not read from socket");
        }
        
        buffer[0] = buffer[1];
        buffer[1] = singleCharBuffer;
        if (raw == SingleLineMessage::RAW or not (buffer[0] == '\r' and buffer[1] == '\n')) {
            finalResult += singleCharBuffer;
        }
        else {
            finalResult.pop_back();
        }
        if (error == false and finalResult[0] == '-') {
            error = true;
        }
    }
    return finalResult;
}

std::string POP3Connection::readMultiLineResponse() {
    std::string finalResult;
    int bytesRead;
    while (true) {
        auto buffer = readLineResponse(SingleLineMessage::RAW);
        if (isError(buffer)) {
            // string is error and thus it'll be singleline. Def return
            return buffer;
        }
        if (buffer == "\r\n") {
            finalResult += "\n";
        }
        buffer.pop_back();
        buffer.pop_back();
        if (buffer == ".") {
            break;
        }
        finalResult += buffer + "\n";
    }
    return finalResult;
}

void POP3Connection::keepAlive() {
    while(true) {
        {
            std::unique_lock<std::mutex> lock(_stateMutex);
            auto result = cv.wait_for(lock, std::chrono::seconds(_noopTimeoutSecs), [this]() {return _state != State::TRANSACTION;});
            if (result == true) {
                break;
            }
        }
        try {
            assertResponse(execCommand("NOOP"));
            log("Sent NOOP");
        }
        catch(...) {
            _hasNoopThreadErrored.store(true);
            log("Error sending NOOP. Exiting NOOP thread");
            return;
        }
    }
}

void POP3Connection::closeConnection() {
    log("Destructing instance...");
    if (_state == DISCONNECTED) {
        log("No need to close connection, already disconnected");
        return;
    }
    {
        std::unique_lock<std::mutex> lock(_stateMutex);
        _state = State::DISCONNECTING;
    }
    cv.notify_all();
    if (_threadStarted) {
        _noopThread.join();
        log("Joined no-op thread");
        _threadStarted = false;
    }
    closeSocket();
    _state = DISCONNECTED;
    log("Successfully closed connection");
    _hasNoopThreadErrored.store(false);
    _isUIDLAvailable = true;
}

void POP3Connection::quitConnection() {
    log("Closing connection...");
    if (_state == DISCONNECTED) {
        log("No need to close connection, already disconnected");
        return;
    }
    State oldState;
    {
        std::unique_lock<std::mutex> lock(_stateMutex);
        oldState = _state;
        _state = State::DISCONNECTING;
    }
    cv.notify_all();
    if (_threadStarted) {
        _noopThread.join();
        log("Joined no-op thread");
        _threadStarted = false;
    }

    if (oldState != DISCONNECTED and oldState != DISCONNECTING) {
        execCommand("QUIT");
        log("Successfully quit connection");
        _state = DISCONNECTING;
    }
    
    closeSocket();
    _state = DISCONNECTED;
    log("Successfully closed connection");
    _isUIDLAvailable = true;
}

std::string POP3Connection::retrieveOneMail(size_t currentMailIndex, size_t byteSize) {
    checkNoopThreadError();

    std::lock_guard<std::mutex> lock(_commandMutex);

    sendCommand("RETR " + std::to_string(currentMailIndex));
    readLineResponse();
    char* buffer = new char[byteSize + 1];

    buffer[byteSize] = 0;
    
    checkNoopThreadError();
    int readCode = readFromSocket(buffer, byteSize);
    if (readCode <= 0) {
        throw ConnectException("Could not read from socket");
    }
    
    readMultiLineResponse();
    std::string x = buffer;

    delete[] buffer;
    return x;
}

std::vector<RawMailMetadata> POP3Connection::retrieveAllMailMetadata() {
    auto serverResponse = execCommand("LIST", true);
    std::vector<RawMailMetadata> mailVector;

    std::string buffer;
    size_t messageNumber;

    std::istringstream responseStream(serverResponse);
    std::string firstLine;
    std::getline(responseStream, firstLine);
    std::istringstream firstLineStream(firstLine);
    firstLineStream >> buffer;
    
    int index, byteSize;
    std::string restOfLine;
    while (responseStream >> index >> byteSize) {
        std::getline(responseStream, restOfLine);
        mailVector.push_back(RawMailMetadata(index, byteSize));
    }
    return mailVector;
}

std::string POP3Connection::retrieveOneMailUIDL(size_t currentMailIndex) {
    auto UIDLResponse = execCommand("UIDL " + std::to_string(currentMailIndex), false, PROCESSED);
    std::istringstream uidlStream(UIDLResponse);
    std::string responsePrefix, mailIndex, UIDL;
    uidlStream >> responsePrefix >> mailIndex >> UIDL;
    return UIDL;
}

void POP3Connection::resetConnection() {
    bool shouldLogin = (_state == State::TRANSACTION);
    quitConnection();
    connectToServer();
    if (shouldLogin) {
        login(_user, _pass);
    }
}

void POP3Connection::markMailForDeletion(long idx) {
    if (idx < 0) {
        throw ServerException("Invalid mail index to delete");
    }
    execCommand("DELE " + std::to_string(idx));
}

bool POP3Connection::isError(const std::string& message) {
    return message[0] == '-';
}

void POP3Connection::assertResponse(const std::string& response) {
    if (isError(response)) {
        throw ServerResponseException(response.substr(5));
    }
}