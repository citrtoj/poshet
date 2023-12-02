#include "Pop3Connection.hpp"

Pop3Connection::Pop3Connection(const char* host, const char* port) :
    _host(host),
    _port(port) {}

int Pop3Connection::openSocket() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        return -1;
    }
    return 0;
}

int Pop3Connection::connectToPop3Server() {
    if (this->openSocket() == -1) {
        return -1;
    }
    struct addrinfo *result;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    int status = getaddrinfo(_host, _port, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    status = connect(_socket, result->ai_addr, (int)result->ai_addrlen);

    if (status != 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);
    std::cout << "connected!\n";
    char buffer[512];
    read(_socket, buffer, 512);
    std::cout << buffer;
    _state = POP3::State::TRANSACTION;
    return 0;
}

std::tuple<int, std::string> Pop3Connection::execCommand(const std::string& command, bool expectsMultiline) {
    std::string tmpCommand = command + "\r\n";  //CRLF ending
    int status = Utils::writeLoop(_socket, tmpCommand.c_str(), tmpCommand.length());
    if (status < 0) {
        return {status, "Error writing command to POP3 server"};
    }

    if (!expectsMultiline) {
        return readSingleLineResponse();
    }
    else {
        return readMultiLineResponse();
    }
}

std::tuple<int, std::string> Pop3Connection::readSingleLineResponse() {
    char buffer[POP3::DEFAULT_SIZE];
    std::string finalResult = "";

    size_t found = finalResult.find("\r\n");
    while (found == std::string::npos) {
        // attempt reading from server
        memset(buffer, 0, POP3::DEFAULT_SIZE);
        int status = read(_socket, buffer, POP3::DEFAULT_SIZE);
        if (status == 0) {
            return {-1, "Connection closed."};
        }
        finalResult += buffer;
        found = finalResult.find("\r\n");
    }
    return {0, finalResult};
}

std::tuple<int, std::string> Pop3Connection::readMultiLineResponse() {
    char buffer[POP3::DEFAULT_SIZE];
    // TODO

    return {0, ""};
}