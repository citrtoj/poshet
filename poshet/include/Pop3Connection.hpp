
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

#include "Utils.hpp"
#include "DatabaseConnection.hpp"
#include "Exceptions.hpp"

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
        DISCONNECTING,
        DISCONNECTED,
        AUTHORIZATION,  // after opening socket
        TRANSACTION
    };

    enum SingleLineMessage { // should readSingleLineMessage replace \r\n with \n or not?
        PROCESSED,
        RAW
    };

    static constexpr int DEFAULT_SIZE = 512 + 1;
    static constexpr int TIMEOUT_SECS = 180;
}

class Pop3Connection {
protected:
    int _socket;
    
    std::string _host, _port = "110";
    std::string _user, _pass;

    POP3::State _state = POP3::State::DISCONNECTED;

    std::mutex _mutex;
    std::thread noopThread;
    std::condition_variable cv;
    bool shouldExitNoopThread;

    void openSocket();
    std::string readLineResponse(bool raw = POP3::SingleLineMessage::PROCESSED);
    std::string readMultiLineResponse();

    void keepAlive();

public:
    Pop3Connection(const std::string& host);
    Pop3Connection();
    ~Pop3Connection();

    void setHost(const std::string& host);

    void connectToPop3Server();
    void login(const std::string& user, const std::string& pass);
    std::string execCommand(const std::string& command, bool expectsMultiline = false);

    void closeConnection();
};