#pragma once
#include <string>


class SMTPConnection {
protected:
    std::string _host;
    std::string _port = "25"; 
    std::string _clientDomain = "localhost";

    bool _hostSet = false;

    int _socket;
public:
    SMTPConnection(const std::string& host);
    SMTPConnection();

    void setHost(const std::string& host);
    void connect();
};