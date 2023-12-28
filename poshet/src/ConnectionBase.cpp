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
    _isSocketOpen = true;
}

void ConnectionBase::closeSocket() {
    if (!isSocketOpen()) {
        return;
    }
    if (_isSSLEnabled) {
        SSL_shutdown(_ssl);
        SSL_free(_ssl);
        SSL_CTX_free(_ctx);
        _isSSLInit = false;
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
    if (_isSSLEnabled) {
        initSSL();
        SSL_set_options(_ssl, SSL_OP_ALL);
        SSL_set_fd(_ssl, _socket);
        if (SSL_connect(_ssl) != 1) {
            auto sslError = SSL_get_error(_ssl, -1);
            throw ConnectException("Could not establish SSL handshake (error code: " + std::to_string(sslError) + ")");
        }
    }
    _isSocketConnected = true;
}

ConnectionBase::~ConnectionBase() {
    closeSocket();
}

ssize_t ConnectionBase::readFromSocket(void* buffer, size_t nbytes) {
    // a direct violation of the open/closed principle but I'm probably not going to read data from this class in literally any other way
    if (_isSSLEnabled) {
        return Utils::readLoopSSL(_ssl, buffer, nbytes);
    }
    else {
        return Utils::readLoop(_socket, buffer, nbytes);
    }
}

ssize_t ConnectionBase::writeToSocket(const void* buffer, size_t nbytes) {
    if (_isSSLEnabled) {
        return Utils::writeLoopSSL(_ssl, buffer, nbytes);
    }
    else {
        return Utils::writeLoop(_socket, buffer, nbytes);
    }
}