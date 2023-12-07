#include "Pop3Connection.hpp"

Pop3Connection::Pop3Connection(const std::string& host) :
    _host(host)
{}

Pop3Connection::Pop3Connection() : Pop3Connection("localhost") {}

void Pop3Connection::setHost(const std::string& host) {
    _host = host;
}

void Pop3Connection::openSocket() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1) {
        throw Exception("Could not initialize socket");
    }
}

void Pop3Connection::connectToPop3Server(const std::string& user, const std::string& pass) {
    if (_state != POP3::DISCONNECTED) {
        return;
    }
    this->openSocket();
    struct addrinfo *result;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    int status = getaddrinfo(_host.c_str(), _port.c_str(), &hints, &result);
    if (status != 0) {
        throw Exception("Could not get address info of server");
    }
    status = connect(_socket, result->ai_addr, (int)result->ai_addrlen);

    if (status != 0) {
        throw Exception("Could not connect to server");
    }

    freeaddrinfo(result);
    readLineResponse();

    noopThread = std::thread(&Pop3Connection::keepAlive, this);
}

std::string Pop3Connection::execCommand(const std::string& command, bool expectsMultiline) {
    std::lock_guard<std::mutex> lock(_mutex);
    std::string tmpCommand = command + "\r\n";  //CRLF ending
    int status = Utils::writeLoop(_socket, tmpCommand.c_str(), tmpCommand.length());
    if (status < 0) {
        throw Exception("Error writing command to server");
    }
    if (!expectsMultiline) {
        return readLineResponse(POP3::SingleLineMessage::PROCESSED);
    }
    else {
        return readMultiLineResponse();
    }
}

std::string Pop3Connection::readLineResponse(bool raw) {
    std::string finalResult = "";
    bool error = false;

    char buffer[2] = {0, 0};
    while (not (buffer[0] == '\r' and buffer[1] == '\n')) {
        char singleCharBuffer;
        int readCode = Utils::readLoop(_socket, &singleCharBuffer, 1);
        if (readCode <= 0) {
            throw Exception("Error reading character from socket.");
        }
        buffer[0] = buffer[1];
        buffer[1] = singleCharBuffer;
        if (raw == POP3::SingleLineMessage::RAW or not (buffer[0] == '\r' and buffer[1] == '\n')) {
            finalResult += singleCharBuffer;
        }
        else {
            finalResult.pop_back();
        }
        
        if (finalResult[0] == '-') {
            error = true;
        }
    }
    if (error) {  // contains "-ERR ..."
        throw ServerException(finalResult.substr(5));
    }
    return finalResult;
}

std::string Pop3Connection::readMultiLineResponse() {
    std::string finalResult;
    int bytesRead;
    while (true) {
        auto buffer = readLineResponse(POP3::SingleLineMessage::RAW);
        if (buffer == "\r\n") {
            finalResult += "\n";
        }
        buffer.pop_back();
        buffer.pop_back();
        if (buffer == ".") {
            break;
        }
        finalResult += buffer + "\n";
    }
    return finalResult;
}

void Pop3Connection::keepAlive() {
    std::cout << "Started noop thread\n";
    while(true) {
        execCommand("NOOP");
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (cv.wait_for(lock, std::chrono::seconds(POP3::TIMEOUT_SECS), [this] { return _state == POP3::State::DISCONNECTING; })) {
                break;
            }
        }
    }
}

void Pop3Connection::closeConnection() {
    if (_state == POP3::DISCONNECTED) {
        return;
    }
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _state = POP3::State::DISCONNECTING;
    }
    cv.notify_all();
    noopThread.join();
    close(_socket);
    _state = POP3::DISCONNECTED;
}

// std::vector<std::string> Pop3Connection::retrieveAllMail() {
//     auto result = execCommand("LIST", true);
    
// }