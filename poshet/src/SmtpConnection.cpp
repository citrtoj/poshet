#include "SmtpConnection.hpp"

SMTPConnection::SMTPConnection() {
    setPort("25");
    _nameOfConnection = "SMTP Connection";
}

SMTPConnection::SMTPConnection(const std::string& host) : SMTPConnection() {
    setHost(host);
}

void SMTPConnection::setClientDomain(const std::string& domain) {
    _clientDomain = domain;
}

std::string SMTPConnection::readResponse() {
    std::string finalResult;
    while (true) {
        char buffer[4 + 1];
        int readCode = Utils::readLoop(_socket, buffer, 4);
        if (readCode < 0) {
            throw Exception("Error reading status code from server");
        }
        finalResult += buffer;
        while (!(finalResult[finalResult.size() - 2] == '\r' and finalResult[finalResult.size() - 1] == '\n')) {
            char oneCharBuffer;
            int readCode = Utils::readLoop(_socket, &oneCharBuffer, 1);
            if (readCode < 0) {
                throw Exception("Error reading SMTP response from server");
            }
            finalResult += oneCharBuffer;
        }
        if (buffer[3] == '\r' or buffer[3] == ' ') {
            break;
        } 
    }
    return finalResult;
}

void SMTPConnection::sendCommand(const std::string& command) {
    std::string tmpCommand = command + "\r\n";
    int status = Utils::writeLoop(_socket, tmpCommand.c_str(), tmpCommand.length());
    if (status < 0) {
        throw Exception("Error writing command to server");
    }
}

void SMTPConnection::connectToServer() {
    connectSocket();
    readResponse();
    execCommand("EHLO " + _clientDomain);

    // pot de pe acum sa trimit NOOP in thread
    _noopThread = std::thread(&SMTPConnection::keepAlive, this);
    _threadStarted = true;
}

void SMTPConnection::keepAlive() {
    bool stop = true;
    while(stop) {
        if (_state == DISCONNECTED)
        execCommand("NOOP");
        log("Sent NOOP");
        {
            std::unique_lock<std::mutex> lock(_shouldExitMutex);
            auto result = cv.wait_for(lock, std::chrono::seconds(TIMEOUT_SECS), [this]() {return _state != State::TRANSACTION;});
            if (result == true) {
                stop = false;
            }
        }
    }
}

std::string SMTPConnection::execCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(_commandMutex);

    sendCommand(command);
    return readResponse();
}

void SMTPConnection::closeConnection() {
    log("Closing connection...");
    {
        std::unique_lock<std::mutex> lock(_shouldExitMutex);
        _state = State::DISCONNECTING;
    }
    if (_threadStarted) {
        cv.notify_all();
        _noopThread.join();
        log("Joined noop thread");
        _threadStarted = false;
    }
    closeSocket();
}

SMTPConnection::~SMTPConnection() {
    closeConnection();
}