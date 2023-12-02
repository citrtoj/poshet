#pragma once

#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "Utils.hpp"

#include <string>
#include <iostream>
#include <unordered_map>

#include <tuple>

namespace POP3 {
    enum Command {
        UNDEFINED,
        QUIT,
        STAT,
        LIST,
        RETR,
        DELE,
        NOOP,
        RSET,
        UIDL,
        USER,
        PASS
    };

    enum State {
        AUTHORIZATION,
        TRANSACTION,
        UPDATE  //after issuing QUIT from TRANSACTION
    };


    static constexpr int DEFAULT_SIZE = 512 + 1;
}

class Pop3Connection {
protected:
    int _socket;
    const char* _host;
    const char* _port;
    POP3::State _state;

    bool _connected;

    int openSocket();


public:
    Pop3Connection(const char* host, const char* port);
    int connectToPop3Server();

    std::tuple<int, std::string> execCommand(const std::string& command, bool expectsMultiline = false);
    std::tuple<int, std::string> readSingleLineResponse();
    std::tuple<int, std::string> readMultiLineResponse();

};