#include "ConnectionBase.hpp"

void ConnectionBase::assertParamChangeDuringActiveConnection() {
    if (_isSocketConnected) {
        throw ConnectException("Cannot change connection parameters while connection is active");
    }
}

void ConnectionBase::setHost(const std::string& host) {
    assertParamChangeDuringActiveConnection();
    _host = host;
}

void ConnectionBase::setPort(const std::string& port) {
    assertParamChangeDuringActiveConnection();
    _port = port;
}

void ConnectionBase::setSSL(bool value) {
    _SSL = value;
}

void ConnectionBase::log(const std::string& logMessage) {
    std::cout << "[" + _nameOfConnection + "] " + logMessage + "\n"; 
}

void ConnectionBase::openSocket(int domain, int type, int protocol) {
    if (isSocketOpen()) {
        return;
    }
    _socket = socket(domain, type, protocol);
    if (_socket == -1) {
        throw ConnectException("Could not initialize socket");
    }
    _isSocketOpen = true;
}

void ConnectionBase::closeSocket() {
    if (!isSocketOpen()) {
        return;
    }
    close(_socket);
    _isSocketConnected = false;
    _isSocketOpen = false;
}

bool ConnectionBase::isSocketOpen() {
    return _isSocketOpen;
}

void ConnectionBase::connectSocket() {
    if (_host.empty()) {
        throw ConnectException("Cannot resolve empty server address");
    }
    struct addrinfo *result, *resultIt;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    int status = getaddrinfo(_host.c_str(), _port.c_str(), &hints, &result);
    if (status != 0) {
        throw ConnectException("Could not resolve address of server (" + _host + ":" + _port + ")");
    }
    for (resultIt = result; resultIt != NULL; resultIt = resultIt->ai_next) {
        try {
            openSocket(resultIt->ai_family, resultIt->ai_socktype, resultIt->ai_protocol);
        }
        catch (ConnectException& e) {
            continue;
        }
        if (connect(_socket, resultIt->ai_addr, resultIt->ai_addrlen) != -1) {
            break;
        }
        close(_socket);
    }

    if (resultIt == NULL) {
        throw ConnectException("Could not connect to server (" + _host + ":" + _port + ")");
    }
    freeaddrinfo(result);
    _isSocketConnected = true;
}

ConnectionBase::~ConnectionBase() {
    closeSocket();
}