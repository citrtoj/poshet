#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <vector>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "Utils.hpp"
#include "Exceptions.hpp"

class ConnectionBase {
protected:
    std::string _nameOfConnection;

    void assertParamChangeDuringActiveConnection();

    int _socket;
    bool _isSocketOpen = false;
    bool _isSocketConnected = false;
    bool _isSSLConnected = false;

    bool _isSSLEnabled = false;
    SSL_CTX* _ctx;
    SSL* _ssl;


    std::string _host;
    std::string _port;

    void openSocket(int domain, int type, int protocol);
    void closeSocket();
    bool isSocketOpen();
    void connectSocket();

    ssize_t readFromSocket(void* buffer, size_t nbytes);
    ssize_t writeToSocket(const void* buffer, size_t nbytes);

    enum State {
        DISCONNECTED,
        DISCONNECTING,
        AUTHORIZATION,
        TRANSACTION
    };
    enum SingleLineMessage { // should readSingleLineMessage replace \r\n with \n or not?
        PROCESSED,
        RAW
    };
    int _timeoutSecs = 60;

public:
    static bool _isOpenSSLInit;
    void InitializeOpenSSL();

    ConnectionBase() {
        InitializeOpenSSL();
    }

    bool _isSSLInit = false;
    void initSSL();

    void setHost(const std::string& host);
    void setPort(const std::string& port);

    void setSSL(bool value = true);
    bool getSSL() const {
        return _isSSLEnabled;
    }

    virtual void connectToServer() = 0;
    virtual void closeConnection() = 0;
    void checkIfConnectionAlive();

    virtual void resetConnection() = 0;

    void log(const std::string& logMessage);
    ~ConnectionBase();
};

class ConnectException : public Exception {
public:
    ConnectException(const std::string& message) : Exception(message) {}
};

class ServerException : public Exception {
public:
    ServerException(const std::string& message) : Exception(message) {}
};

class ServerResponseException : public ServerException {
public:
    ServerResponseException(const std::string& message) : ServerException(message) {}
};

class IOException : public Exception {
public:
    IOException(const std::string& message) : Exception(message) {}
};