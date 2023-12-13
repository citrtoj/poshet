#include "SmtpConnection.hpp"

SMTPConnection::SMTPConnection() {
    _port = "25";
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
    std::cout << readResponse();
    std::cout << execCommand("EHLO " + _clientDomain);
}


std::string SMTPConnection::execCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(_commandMutex);

    sendCommand(command);
    return readResponse();
}