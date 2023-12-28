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

// project-specific includes
#include "Utils.hpp"
#include "Exceptions.hpp"

class ConnectionBase {
protected:
    std::string _nameOfConnection;

    void assertParamChangeDuringActiveConnection();

    int _socket;
    bool _isSocketOpen = false;
    bool _isSocketConnected = false;

    bool _SSL = false;

    std::string _host;
    std::string _port;

    void openSocket(int domain, int type, int protocol);
    void closeSocket();
    bool isSocketOpen();
    void connectSocket();

public:
    ConnectionBase() {}

    void setHost(const std::string& host);
    void setPort(const std::string& port);
    void setSSL(bool value = true);

    virtual void connectToServer() = 0;
    virtual void closeConnection() = 0;

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