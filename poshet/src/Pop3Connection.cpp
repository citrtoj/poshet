#include "Pop3Connection.hpp"

POP3Connection::POP3Connection() {
    setPort("110"); //potentially modify to get the default from config etc etc
}

POP3Connection::POP3Connection(const std::string& host) : POP3Connection() {
    setHost(host);
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
    readLineResponse();
    _state = AUTHORIZATION;
}

void POP3Connection::login() {
    if (_state == TRANSACTION) {
        std::cout << "[POP3] Warning: already logged in!\n";
        return;
    }
    if (_state != AUTHORIZATION) {
        throw Exception("Unable to log in -- not connected to server");
    }
    //todo: check if there isn't perhaps a risk of injection here... remove newlines, or something
    execCommand("USER " + _user);
    execCommand("PASS " + _pass);
    _state = TRANSACTION;
    noopThread = std::thread(&POP3Connection::keepAlive, this);
}

void POP3Connection::login(const std::string& user, const std::string& pass) {
    if (_state == TRANSACTION) {
        std::cout << "[POP3] Warning: already logged in!\n";
        return;
    }
    _user = user;
    _pass = pass;
    login();
}

void POP3Connection::sendCommand(const std::string& command) {
    // todo: DUPLICATE in smtp -- worth moving over to ConnectionBase, perhaps?
    std::string tmpCommand = command + "\r\n";  //CRLF ending
    int status = Utils::writeLoop(_socket, tmpCommand.c_str(), tmpCommand.length());
    if (status < 0) {
        throw Exception("Error writing command to server");
    }
}

std::string POP3Connection::execCommand(const std::string& command, bool expectsMultiline, SingleLineMessage processing) {
    std::lock_guard<std::mutex> lock(_commandMutex);
    sendCommand(command);
    if (!expectsMultiline) {
        auto x = readLineResponse(processing);
        return x;
    }
    else {
        return readMultiLineResponse();
    }
}

std::string POP3Connection::readLineResponse(bool raw) {
    std::string finalResult = "";
    bool error = false;

    char buffer[2] = {0, 0};
    while (not (buffer[0] == '\r' and buffer[1] == '\n')) {
        char singleCharBuffer;
        int readCode = Utils::readLoop(_socket, &singleCharBuffer, 1);
        if (readCode <= 0) {
            throw Exception("Error reading character from socket.");
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
    // TODO -  move this from here to wherever we handle input
    // maybe sometimes we don't simply want to exception out of this

    if (error) {  // contains "-ERR ..."
        throw ServerException(finalResult.substr(5));
    }
    return finalResult;
}

std::string POP3Connection::readMultiLineResponse() {
    std::string finalResult;
    int bytesRead;
    while (true) {
        auto buffer = readLineResponse(SingleLineMessage::RAW);
        if (buffer.find("-ERR") != std::string::npos) {
            // todo: same as a bit above
            throw ServerException(buffer.substr(5));
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
    bool stop = true;
    while(stop) {
        if (_state == DISCONNECTED)
        execCommand("NOOP");
        std::cout << "[POP3] Sent NOOP\n";
        {
            std::unique_lock<std::mutex> lock(_shouldExitMutex);
            auto result = cv.wait_for(lock, std::chrono::seconds(TIMEOUT_SECS), [this]() {return _state != State::TRANSACTION;});
            if (result == true) {
                stop = false;
            }
        }
    }
}

void POP3Connection::closeConnection() {
    std::cout << "[POP3] Closing connection...\n";
    if (_state == DISCONNECTED) {
        std::cout << "[POP3] No need to, already disconnected\n";
        return;
    }
    {
        std::unique_lock<std::mutex> lock(_shouldExitMutex);
        _state = State::DISCONNECTING;
    }
    cv.notify_all();
    noopThread.join();
    std::cout << "[POP3] Joined noop thread\n";
    closeSocket();
    _state = DISCONNECTED;
}

std::string POP3Connection::retrieveOneMail(size_t currentMailIndex, size_t byteSize) {
    std::lock_guard<std::mutex> lock(_commandMutex);

    sendCommand("RETR " + std::to_string(currentMailIndex));
    readLineResponse();
    char* buffer = new char[byteSize + 1];

    buffer[byteSize] = 0;
    int readCode = Utils::readLoop(_socket, buffer, byteSize);
    // todo: handle readCode!!!
    readMultiLineResponse(); // to read whatever other termination chars are at the end
    std::string x = buffer;

    delete[] buffer;

    return x;
}

std::string POP3Connection::retrieveOneMailHeader(size_t currentMailIndex) {
    auto x = execCommand("TOP " + std::to_string(currentMailIndex) + " 0", true);
    // todo: trim "+ OK"
    return x;
}

std::vector<POP3Connection::RawMailData> POP3Connection::retrieveAllMailHeaders() {
    auto serverResponse = execCommand("LIST", true);
    std::vector<RawMailData> mailVector;

    std::string buffer;
    size_t messageNumber;

    std::stringstream strm(serverResponse, std::ios_base::in);
    strm >> buffer >> messageNumber >> buffer;
    
    for (int i = 1; i <= messageNumber; ++i) {
        int index, byteSize;
        strm >> index >> byteSize;
        mailVector.push_back(RawMailData(index, byteSize));
        try {
            mailVector.back().plainData = retrieveOneMailHeader(mailVector.back().index);
        }
        catch (Exception& e) {
            mailVector.pop_back();
        }   
    }
    return mailVector;
}

std::vector<POP3Connection::RawMailData> POP3Connection::retrieveAllMail() {
    auto serverResponse = execCommand("LIST", true);
    std::vector<RawMailData> mailVector;

    std::string buffer;
    size_t messageNumber;

    std::stringstream strm(serverResponse, std::ios_base::in);
    strm >> buffer >> messageNumber >> buffer;
    
    for (int i = 1; i <= messageNumber; ++i) {
        int index, byteSize;
        strm >> index >> byteSize;
        mailVector.push_back(RawMailData(index, byteSize));
        try {
            mailVector.back().plainData = retrieveOneMail(mailVector.back().index, mailVector.back().byteSize);
        }
        catch (Exception& e) {
            mailVector.pop_back();
        }
        
    }
    return mailVector;
}

void POP3Connection::resetConnection() {
    bool shouldLogin = (_state == State::TRANSACTION);
    closeConnection();
    connectToServer();
    if (shouldLogin) {
        login(_user, _pass);
    }
}