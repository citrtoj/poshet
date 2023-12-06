
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
#include "DatabaseConnection.hpp"

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace POP3 {
    class Exception : public std::exception {
    protected:
        std::string _message;
    public:
        Exception(const std::string& message) : _message(message) {}
        const char * what () {
            return _message.c_str();
        }
    };

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
    
    std::string _host, _port;
    std::string _user, _pass;
    bool _needsAuthentication = true;
    POP3::State _state = POP3::State::DISCONNECTED;

    std::mutex _mutex;
    std::thread noopThread;
    std::condition_variable cv;
    bool shouldExitNoopThread;

    int openSocket();
    std::string readLineResponse(bool raw = POP3::SingleLineMessage::PROCESSED);
    std::string readMultiLineResponse();

    //std::string retrieveMail(unsigned int id);

    void keepAlive();

public:
    Pop3Connection(const std::string& host, const std::string& port);
    Pop3Connection();

    void setHost(const std::string& host);
    void setPort(const std::string& port);

    int connectToPop3Server();

    std::string execCommand(const std::string& command, bool expectsMultiline = false);

    std::vector<std::string> retrieveAllMail();

    void closeConnection();
};