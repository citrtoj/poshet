#include "ConnectionBase.hpp"

bool ConnectionBase::_isOpenSSLInit = false;

void ConnectionBase::InitializeOpenSSL() {
    if (_isOpenSSLInit) {
        return;
    }
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    _isOpenSSLInit = true;
}

void ConnectionBase::assertParamChangeDuringActiveConnection() {
    if (_isSocketConnected) {
        throw ConnectException("Cannot change connection parameters while connection is active");
    }
}

void ConnectionBase::initSSL() {
    if (_isSSLInit) {
        return;
    }
    _ctx = SSL_CTX_new(SSLv23_client_method());
    if (!_ctx) {
        throw ConnectException("Error creating SSL context");
    }
    _ssl = SSL_new(_ctx);
    if (!_ssl) {
        throw ConnectException("Error creating SSL connection");
    }
    _isSSLInit = true;
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
    _isSSLEnabled = value;
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
    struct timeval timeout;
    timeout.tv_sec = _timeoutSecs;
    timeout.tv_usec = 0;
    if (setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
        throw ConnectException("Could not set socket recv timeout");
    }
    if (setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
        throw ConnectException("Could not set socket send timeout");
    }
    _isSocketOpen = true;
}

void ConnectionBase::closeSocket() {
    if (!_isSocketOpen) {
        return;
    }
    if (_isSSLInit) {
        if (_isSSLConnected) {
            SSL_shutdown(_ssl);
        }
        SSL_free(_ssl);
        SSL_CTX_free(_ctx);
    }
    _isSSLInit = false;
    _isSSLConnected = false;
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
    hints.ai_family = AF_INET; // enforce IPv4
    hints.ai_socktype = SOCK_STREAM; // enforce TCP
    int status = getaddrinfo(_host.c_str(), _port.c_str(), &hints, &result);
    if (status != 0) {
        throw ConnectException("Could not resolve address of server (" + _host + ":" + _port + "): status code " + std::string(gai_strerror(status)));
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
        closeSocket();
    }

    if (resultIt == NULL) {
        throw ConnectException("Could not connect to server (" + _host + ":" + _port + ")");
    }
    freeaddrinfo(result);
    if (_isSSLEnabled) {
        initSSL();
        SSL_set_options(_ssl, SSL_OP_ALL);
        SSL_set_fd(_ssl, _socket);
        if (SSL_connect(_ssl) != 1) {
            auto sslError = SSL_get_error(_ssl, -1);
            throw ConnectException("Could not establish SSL handshake (error code: " + std::to_string(sslError) + ")");
        }
        _isSSLConnected = true;
    }
    _isSocketConnected = true;
}

ConnectionBase::~ConnectionBase() {
    closeSocket();
}

ssize_t ConnectionBase::readFromSocket(void* buffer, size_t nbytes) {
    try {
        checkIfConnectionAlive();
    }
    catch (ConnectException& e) {
        try {
            resetConnection();
        }
        catch (...) {
            throw ConnectException("Connection failed, could not reconnect");
        }
    }
    if (_isSSLEnabled) {
        return Utils::readLoopSSL(_ssl, buffer, nbytes);
    }
    else {
        return Utils::recvLoop(_socket, buffer, nbytes);
    }
}

ssize_t ConnectionBase::writeToSocket(const void* buffer, size_t nbytes) {
    try {
        checkIfConnectionAlive();
    }
    catch (ConnectException& e) {
        try {
            resetConnection();
        }
        catch (...) {
            throw ConnectException("Connection failed, could not reconnect");
        }
    }
    if (_isSSLEnabled) {
        return Utils::writeLoopSSL(_ssl, buffer, nbytes);
    }
    else {
        return Utils::sendLoop(_socket, buffer, nbytes);
    }
}

void ConnectionBase::checkIfConnectionAlive() {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(_socket, &read_fds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1;
    int result = select(_socket + 1, &read_fds, nullptr, nullptr, &timeout);
    if (result == -1) {
        throw ConnectException("Connection dead");
    }
}