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
#include <sstream>
#include <iterator>

#include "Mail.hpp"
#include "ConnectionBase.hpp"

class RawMailMetadata {
public:
    size_t index;
    size_t byteSize;

    RawMailMetadata(size_t index, size_t byteSize) : index(index), byteSize(byteSize) {}
};

typedef std::string RawMailData;

class RawMail {
public:
    RawMailMetadata metadata;
    RawMailData plainData;
};

class POP3Connection : public ConnectionBase {
protected:
    std::string _user, _pass;
    State _state = State::DISCONNECTED;

    std::thread _noopThread;
    bool _threadStarted = false;
    
    std::mutex _commandMutex;
    std::mutex _stateMutex;
    std::condition_variable cv;

    bool _isUIDLValid = true;

    void sendCommand(const std::string& command); // NOT THREAD SAFE

    std::string _ehloResponse;

    std::string readLineResponse(bool raw = SingleLineMessage::PROCESSED);
    std::string readMultiLineResponse();

    void keepAlive();

    std::string execCommand(const std::string& command, bool expectsMultiline = false, SingleLineMessage processing = SingleLineMessage::PROCESSED); // THREAD SAFE
    
    // std::string retrieveOneMailHeader(size_t currentMailIndex);

public:
    POP3Connection();
    ~POP3Connection();

    static constexpr int DEFAULT_PORT = 110;
    static constexpr int DEFAULT_SSL_PORT = 995;

    static int DefaultPort(bool SSL = false);

    void setUser(const std::string& user);
    void setPass(const std::string& pass);

    void connectToServer() override;
    void closeConnection() override;
    void quitConnection(); // acts as a commit for DELE commands
    void resetConnection();

    void login();
    void login(const std::string& user, const std::string& pass);

    std::vector<RawMailMetadata> retrieveAllMailMetadata();

    void markMailForDeletion(long idx);

    static bool isError(const std::string& message);
    static void assertResponse(const std::string& response);

    std::string retrieveOneMailUIDL(size_t currentMailIndex);
    std::string retrieveOneMail(size_t currentMailIndex, size_t byteSize);
};