#include "Pop3Connection.hpp"

Pop3Connection::Pop3Connection() {
    setPort("110"); //modify to get the default from the.. whatever
}

Pop3Connection::Pop3Connection(const std::string& host) : Pop3Connection() {
    setHost(host);
}

Pop3Connection::~Pop3Connection() {
    closeConnection();
}

void Pop3Connection::setUser(const std::string& user) {
    _user = user;
}

void Pop3Connection::setPass(const std::string& pass) {
    _pass = pass;
}

void Pop3Connection::connectToServer() {
    if (_state != POP3::DISCONNECTED) {
        return;
    }
    connectSocket();
    readLineResponse();
    _state = POP3::AUTHORIZATION;
}

void Pop3Connection::login(const std::string& user, const std::string& pass) {
    if (_state == POP3::TRANSACTION) {
        std::cout << "[POP3] Warning: already logged in!\n";
        return;
    }
    if (_state != POP3::AUTHORIZATION) {
        throw Exception("Unable to log in -- not connected to server");
    }
    execCommand("USER " + user); //todo: check if there isn't perhaps a risk of injection here
    execCommand("PASS " + pass);
    _state = POP3::TRANSACTION;
    noopThread = std::thread(&Pop3Connection::keepAlive, this);
}

std::string Pop3Connection::execCommand(const std::string& command, bool expectsMultiline, POP3::SingleLineMessage processing) {
    std::lock_guard<std::mutex> lock(_commandMutex);
    std::string tmpCommand = command + "\r\n";  //CRLF ending
    int status = Utils::writeLoop(_socket, tmpCommand.c_str(), tmpCommand.length());
    if (status < 0) {
        throw Exception("Error writing command to server");
    }
    if (!expectsMultiline) {
        auto x = readLineResponse(processing);
        return x;
    }
    else {
        return readMultiLineResponse();
    }
}

std::string Pop3Connection::readLineResponse(bool raw) {
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
        if (raw == POP3::SingleLineMessage::RAW or not (buffer[0] == '\r' and buffer[1] == '\n')) {
            finalResult += singleCharBuffer;
        }
        else {
            finalResult.pop_back();
        }
        
        if (finalResult[0] == '-') {
            error = true;
        }
    }
    if (error) {  // contains "-ERR ..."
        throw ServerException(finalResult.substr(5));
    }
    return finalResult;
}

std::string Pop3Connection::readMultiLineResponse() {
    std::string finalResult;
    int bytesRead;
    while (true) {
        auto buffer = readLineResponse(POP3::SingleLineMessage::RAW);
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

void Pop3Connection::keepAlive() {
    bool stop = true;
    while(stop) {
        execCommand("NOOP");
        std::cout << "[POP3] Sent NOOP\n";
        {
            std::unique_lock<std::mutex> lock(_shouldExitMutex);
            auto result = cv.wait_for(lock, std::chrono::seconds(POP3::TIMEOUT_SECS), [this]() {return _state != POP3::State::TRANSACTION;});
            if (result == true) {
                stop = false;
            }
        }
    }
}

void Pop3Connection::closeConnection() {
    std::cout << "[POP3] Closing connection...\n";
    if (_state == POP3::DISCONNECTED) {
        std::cout << "Already disconnected!\n";
        return;
    }
    {
        std::unique_lock<std::mutex> lock(_shouldExitMutex);
        _state = POP3::State::DISCONNECTING;
    }
    cv.notify_all();
    std::cout << "[POP3] Notified cv\n";
    noopThread.join();
    std::cout << "[POP3] Joined noop thread!!\n";
    closeSocket();
    _state = POP3::DISCONNECTED;
}

std::vector<POP3::MailData> Pop3Connection::retrieveMail() {
    auto serverResponse = execCommand("LIST", true);
    std::cout << serverResponse << "\n";
    std::vector<POP3::MailData> mailVector;

    std::string buffer;
    size_t messageNumber;

    std::stringstream strm(serverResponse, std::ios_base::in);
    strm >> buffer >> messageNumber >> buffer;
    
    for (int i = 1; i <= messageNumber; ++i) {
        int index, byteSize;
        strm >> index >> byteSize;
        std::cout << index << " " << byteSize << "\n";
        mailVector.push_back(POP3::MailData(index, byteSize));
        // incercam sa citim aici
        // auto serverMailResponse = execCommand("RETR " + std::to_string(i), false);
        // {
        //     std::cout << "here\n";
        //     std::lock_guard<std::mutex> lock(_commandMutex);
        //     // I still have to read byteSize bytes
        //     char* mailBuffer = new char[byteSize + 1];
        //     int readCode = Utils::readLoop(_socket, mailBuffer, byteSize);
        //     mailBuffer[byteSize] = 0;
        //     std::cout << mailBuffer;
        //     delete mailBuffer;
        // }
    }

    return mailVector;
}