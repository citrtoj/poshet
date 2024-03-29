#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>

#include "ConnectionBase.hpp"

class SMTPConnection : public ConnectionBase {
public:
    enum LoginType {
        NONE,
        PLAIN,
        LOGIN
    };
protected:
    std::string _user, _pass;
    LoginType _loginType = NONE;
    std::string _clientDomain = "localhost";
    
    State _state = DISCONNECTED;
    std::string _ehloResponse;

    void sendCommand(const std::string& command, bool addCRLF = true); // NOT THREAD SAFE; only sends the command, without reading response

    std::string execCommand(const std::string& command, bool addCRLF = true); // THREAD SAFE; sends commands, returns response
    
    std::unordered_map<LoginType, std::function<void()>> _loginMethods;
    void initLoginMethods();

public:
    SMTPConnection(const std::string& host);
    SMTPConnection();
    ~SMTPConnection();

    static constexpr int DEFAULT_PORT = 25;
    static constexpr int DEFAULT_SSL_PORT = 465;

    static int DefaultPort(bool SSL = false);

    void setClientDomain(const std::string& domain);

    void connectToServer() override;
    void closeConnection() override;

    void resetConnection() override;

    void login();
    void login(const std::string& user, const std::string& pass);

    std::string readResponse();
    void assertResponse(const std::string& response);

    void sendMail(const std::string& from, const std::string& to, const std::string& rawBody);
};