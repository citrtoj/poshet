#pragma once
#include <string>


class SMTPConnection {
protected:
    std::string _serverDomain;
    std::string _port;
    std::string _clientDomain;

    int _socket;
public:
    SMTPConnection();
};