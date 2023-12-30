#include "SmtpConnection.hpp"

int SMTPConnection::DefaultPort(bool SSL) {
    if (SSL) {
        return DEFAULT_SSL_PORT;
    }
    else {
        return DEFAULT_PORT;
    }
}

SMTPConnection::SMTPConnection() {
    _nameOfConnection = "SMTP Connection";
    setPort(std::to_string(DefaultPort(_isSSLEnabled)));
    initLoginMethods();
}

SMTPConnection::SMTPConnection(const std::string& host) : SMTPConnection() {
    setHost(host);
}

void SMTPConnection::initLoginMethods() {
    _loginMethods = { {
            PLAIN, [this]() {
                throw ConnectException("PLAIN Login method not implemented");
            }
        }, {
            LOGIN, [&]() {
                log(execCommand("AUTH LOGIN"));

                // todo: assert commands, check for errors in message

                auto base64User = Utils::encodeToBase64(_user + "\n");
                log(execCommand(base64User, false));
                auto base64Pass = Utils::encodeToBase64(_pass + "\n");
                log(execCommand(base64Pass, false));
            }
        }
    };
}

void SMTPConnection::setClientDomain(const std::string& domain) {
    _clientDomain = domain;
}

std::string SMTPConnection::readResponse() {
    std::string finalResult = "";
    while (true) {
        char buffer[4 + 1] = {0};
        int readCode = readFromSocket(buffer, 4);
        if (readCode < 0) {
            throw Exception("Error reading status code from server");
        }
        finalResult += buffer;
        while (!(finalResult[finalResult.size() - 2] == '\r' and finalResult[finalResult.size() - 1] == '\n')) {
            char oneCharBuffer;
            int readCode = readFromSocket(&oneCharBuffer, 1);
            if (readCode < 0) {
                throw Exception("Error reading SMTP response from server");
            }
            finalResult += oneCharBuffer;
        }
        if (buffer[3] == '\r' or buffer[3] == ' ') {
            break;
        } 
    }
    return finalResult;
}

void SMTPConnection::sendCommand(const std::string& command, bool addCRLF) {
    std::string tmpCommand = command;
    if (addCRLF) {
        tmpCommand += "\r\n";
    }
    int status = writeToSocket(tmpCommand.c_str(), tmpCommand.length());
    if (status < 0) {
        throw Exception("Error writing command to server");
    }
}

void SMTPConnection::connectToServer() {
    connectSocket();
    readResponse();
    _ehloResponse = execCommand("EHLO " + _clientDomain);

    // pot de pe acum sa trimit NOOP in thread
    _noopThread = std::thread(&SMTPConnection::keepAlive, this);
    _threadStarted = true;
    _state = State::TRANSACTION;
}

void SMTPConnection::keepAlive() {
    bool stop = true;
    while(true) {
        {
            std::unique_lock<std::mutex> lock(_stateMutex);
            auto result = cv.wait_for(lock, std::chrono::seconds(_timeoutSecs), [this]() {return _state != State::TRANSACTION;});
            if (result == true) {
                break;
            }
        }
        try {
            execCommand("NOOP");
        }
        catch(Exception& e) {
            std::cout << e.what() << "\n";
            throw;
        }
        log("Sent NOOP");
    }
}

std::string SMTPConnection::execCommand(const std::string& command, bool addCRLF) {
    std::lock_guard<std::mutex> lock(_commandMutex);

    sendCommand(command, addCRLF);
    return readResponse();
}

void SMTPConnection::closeConnection() {
    log("Closing connection...");
    {
        std::unique_lock<std::mutex> lock(_stateMutex);
        _state = State::DISCONNECTING;
    }
    cv.notify_all();
    if (_threadStarted) {
        _noopThread.join();
        log("Joined noop thread");
        _threadStarted = false;
    }
    closeSocket();
}

void SMTPConnection::sendMail(const Mail& mail) {
    try {
        auto to = mail.getHeaderField("To");
        auto from = mail.getHeaderField("From");
        auto content = mail.plainText() + "\r\n.";

        log(execCommand("MAIL FROM: " + from));
        log(execCommand("RCPT TO: " + to));
        log(execCommand("DATA"));
        log(execCommand(content));
    }
    catch (MailException& exc) {
        throw Exception("Could not get needed info in order to send mail");
    }
    catch (ServerException& exc) {
        throw Exception("Server was unable to send mail");
    }
    
}


SMTPConnection::~SMTPConnection() {
    closeConnection();
}

void SMTPConnection::login(const std::string& user, const std::string& pass) {
    _user = user;
    _pass = pass;

    login();
}

void SMTPConnection::login() {
    // get login types from ehlo... see if any are supported by us
    // for now, pretend that all the servers support this
    // todo: actually implement this
    _loginMethods.find(LOGIN)->second();
}