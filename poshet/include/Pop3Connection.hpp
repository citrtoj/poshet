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

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Utils.hpp"
#include "DatabaseConnection.hpp"
#include "Exceptions.hpp"
#include "ConnectionBase.hpp"

class Pop3Connection : public ConnectionBase {
    class RawMailData {
    public:
        size_t index;
        size_t byteSize;
        std::string plainData;

        RawMailData(size_t index, size_t byteSize) : index(index), byteSize(byteSize) {}
        RawMailData() {}
    };
    enum Command {
        // not sure if I'll need these
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
        DISCONNECTED,
        DISCONNECTING,
        AUTHORIZATION,  // after opening socket
        TRANSACTION
    };
    enum SingleLineMessage { // should readSingleLineMessage replace \r\n with \n or not?
        PROCESSED,
        RAW
    };
    static constexpr int TIMEOUT_SECS = 3;
protected:
    std::string _user, _pass;

    State _state = State::DISCONNECTED;

    std::thread noopThread;
    std::mutex _commandMutex;
    std::mutex _shouldExitMutex;
    std::condition_variable cv;

    void sendCommand(const std::string& command);

    std::string readLineResponse(bool raw = SingleLineMessage::PROCESSED);
    std::string readMultiLineResponse();

    void keepAlive();

    std::string execCommand(const std::string& command, bool expectsMultiline = false, SingleLineMessage processing = SingleLineMessage::PROCESSED);

    std::string retrieveOneMail(size_t currentMailIndex, size_t byteSize);
    std::string retrieveOneMailHeader(size_t currentMailIndex);
public:
    Pop3Connection(const std::string& host);
    Pop3Connection();
    ~Pop3Connection();

    void setUser(const std::string& user);
    void setPass(const std::string& pass);

    void connectToServer() override;
    void closeConnection() override;
    void resetConnection();

    void login();
    void login(const std::string& user, const std::string& pass);

    std::vector<RawMailData> retrieveAllMail();
    std::vector<RawMailData> retrieveAllMailHeaders();
};