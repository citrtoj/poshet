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

class POP3Connection : public ConnectionBase {

    class RawMailData {
    public:
        size_t index;
        size_t byteSize;
        std::string plainData;

        RawMailData(size_t index, size_t byteSize) : index(index), byteSize(byteSize) {}
        RawMailData() {}
    };
    enum State {
        DISCONNECTED,
        DISCONNECTING,
        AUTHORIZATION,  // after opening socket
        TRANSACTION
    };
    enum SingleLineMessage { // should readSingleLineMessage replace \r\n with \n or not?
        PROCESSED,
        RAW
    };
    int _timeoutSecs = 60;
protected:

    std::string _user, _pass;

    State _state = State::DISCONNECTED;

    std::thread _noopThread;
    bool _threadStarted = false;
    
    std::mutex _commandMutex;
    std::mutex _stateMutex;
    std::condition_variable cv;

    void sendCommand(const std::string& command); // NOT THREAD SAFE

    std::string readLineResponse(bool raw = SingleLineMessage::PROCESSED);
    std::string readMultiLineResponse();

    void keepAlive();

    std::string execCommand(const std::string& command, bool expectsMultiline = false, SingleLineMessage processing = SingleLineMessage::PROCESSED); // THREAD SAFE

    std::string retrieveOneMail(size_t currentMailIndex, size_t byteSize);
    std::string retrieveOneMailHeader(size_t currentMailIndex);
public:
    POP3Connection(const std::string& host);
    POP3Connection();
    ~POP3Connection();

    void setUser(const std::string& user);
    void setPass(const std::string& pass);

    void connectToServer() override;
    void closeConnection() override;
    void quitConnection(); // acts as a commit
    void resetConnection();

    void login();
    void login(const std::string& user, const std::string& pass);

    std::vector<RawMailData> retrieveAllMail();
    std::vector<RawMailData> retrieveAllMailHeaders();

    void markMailForDeletion(long idx);
};