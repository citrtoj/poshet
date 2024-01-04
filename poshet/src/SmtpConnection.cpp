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
                assertResponse(execCommand("AUTH LOGIN"));
                auto base64User = Utils::encodeToBase64(_user + "\n");
                assertResponse(execCommand(base64User, false));
                auto base64Pass = Utils::encodeToBase64(_pass + "\n");
                assertResponse(execCommand(base64Pass, false));
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

    _state = State::TRANSACTION;
}

std::string SMTPConnection::execCommand(const std::string& command, bool addCRLF) {
    sendCommand(command, addCRLF);
    return readResponse();
}

void SMTPConnection::closeConnection() {
    log("Closing connection...");
    _state = State::DISCONNECTING;
    closeSocket();
    log("Closed connection");
}

void SMTPConnection::sendMail(const std::string& from, const std::string& to, const std::string& rawBody) {
    try {
        auto content = rawBody + "\r\n.";
        assertResponse(execCommand("MAIL FROM:" + from));
        assertResponse(execCommand("RCPT TO:" + to));
        assertResponse(execCommand("DATA"));
        assertResponse(execCommand(content));
    }
    catch (ServerException& e) {
        throw Exception(std::string("Server was unable to send mail (") + e.what() + ")");
        return;
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
    // I could actually implement this... maybe...
    _loginMethods.find(LOGIN)->second();
}

void SMTPConnection::assertResponse(const std::string& response) {
    int buffer;
    std::istringstream x(response);
    x >> buffer;
    std::string serverMessage;
    std::getline(x, serverMessage);

    switch (buffer / 100) {
        case 2:
            log("Note (server status code " + std::to_string(buffer) + "): " + serverMessage);
            break;
        case 3:
            log("Note (server status code " + std::to_string(buffer) + "): " + serverMessage);
            break;
        case 4:
            throw ServerResponseException(serverMessage);
            break;
        case 5:
            throw ServerResponseException("Bad command/command sequence");
            break;
        default:
            throw Exception("Invalid SMTP status code");
    }
}