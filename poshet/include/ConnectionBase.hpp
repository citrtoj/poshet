#pragma once

// POSIX includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>

// C++ stdlib includes>
#include <string>
#include <iostream>
#include <vector>

// project-specific includes
#include "Utils.hpp"
#include "Exceptions.hpp"

class ConnectionBase {
protected:
    std::string _nameOfConnection;

    int _socket;
    bool _isSocketOpen;

    std::string _host;
    std::string _port;

    void openSocket();
    void closeSocket();
    bool isSocketOpen();
    void connectSocket();

public:
    ConnectionBase() {}

    void setHost(const std::string& host);
    void setPort(const std::string& port);

    virtual void connectToServer() = 0;
    virtual void closeConnection() = 0;

    void log(const std::string& logMessage);

    ~ConnectionBase();
};