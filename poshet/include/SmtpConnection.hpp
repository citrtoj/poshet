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

#include "ConnectionBase.hpp"


namespace SMTP {
    enum State {
        DISCONNECTED,
        DISCONNECTING, 
        TRANSACTION
    };
    enum SingleLineMessage { // should readSingleLineMessage replace \r\n with \n or not?
        PROCESSED,
        RAW
    };
    static constexpr int TIMEOUT_SECS = 3;
}

class SMTPConnection : public ConnectionBase {
protected:
    int _socket;

    std::string _clientDomain = "localhost";

    SMTP::State _state = SMTP::State::DISCONNECTED;

    std::thread _noopThread;
    std::mutex _commandMutex;
    std::mutex _shouldExitMutex;
    std::condition_variable cv;

    std::string readLineResponse(bool raw = SMTP::SingleLineMessage::PROCESSED);
    std::string readMultiLineResponse();

    void keepAlive();
public:
    SMTPConnection(const std::string& host);
    SMTPConnection();
    ~SMTPConnection() {}

    void setClientDomain(const std::string& domain);

    void connectToServer() override {}
    void closeConnection() override {}
};