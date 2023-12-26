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

#include "Mail.hpp"
#include "ConnectionBase.hpp"

class SMTPConnection : public ConnectionBase {
    enum State {
        DISCONNECTED,
        DISCONNECTING, 
        TRANSACTION
    };
    enum SingleLineMessage { // should readSingleLineMessage replace \r\n with \n or not?
        PROCESSED,
        RAW
    };
    int _timeoutSecs = 60;
protected:
    std::string _clientDomain = "localhost";
    
    State _state = State::DISCONNECTED;

    bool _enableSSL = false;

    std::thread _noopThread;
    bool _threadStarted = false;
    std::mutex _commandMutex;
    std::mutex _stateMutex;
    std::condition_variable cv;

    void sendCommand(const std::string& command); // NOT THREAD SAFE; only sends the command, without reading response

    std::string execCommand(const std::string& command); // THREAD SAFE; sends commands, returns response

    void keepAlive();
public:
    SMTPConnection(const std::string& host);
    SMTPConnection();
    ~SMTPConnection();

    void setClientDomain(const std::string& domain);

    void connectToServer() override;
    void closeConnection() override;

    std::string readResponse();

    void sendMail(const Mail& mail);
};