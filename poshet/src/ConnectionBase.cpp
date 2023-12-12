#include "ConnectionBase.hpp"

void ConnectionBase::setHost(const std::string& host) {
    _host = host;
}

void ConnectionBase::setPort(const std::string& port) {
    _port = port;
}

void ConnectionBase::openSocket() {
    if (isSocketOpen()) {
        return;
    }
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        throw Exception("Could not initialize socket");
    }
    _isSocketOpen = true;
}

void ConnectionBase::closeSocket() {
    if (!isSocketOpen()) {
        return;
    }
    close(_socket);
    _isSocketOpen = false;
}

bool ConnectionBase::isSocketOpen() {
    return _isSocketOpen;
}

void ConnectionBase::connectSocket() {
    this->openSocket();
    if (_host.empty()) {
        throw Exception("Cannot resolve empty server address");
    }
    struct addrinfo *result;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    int status = getaddrinfo(_host.c_str(), _port.c_str(), &hints, &result);
    if (status != 0) {
        throw Exception("Could not resolve address of server (" + _host + ":" + _port + ")");
    }
    status = connect(_socket, result->ai_addr, (int)result->ai_addrlen);

    if (status != 0) {
        throw Exception("Could not connect to server (" + _host + ":" + _port + ")");
    }
    freeaddrinfo(result);
}

ConnectionBase::~ConnectionBase() {
    closeSocket();
}