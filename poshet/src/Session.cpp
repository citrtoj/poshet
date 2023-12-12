#include "Session.hpp"

void Session::setLoginData(const LoginData& data) {
    std::cout << "[Session] Setting login data\n";
    _loginData = data;
    std::cout << "[Session] Setting POP3 domain\n";
    _pop3.setHost(_loginData.pop3Domain());
    _smtp.setHost(_loginData.smtpDomain());
    std::cout << "[Session] Set login data\n";
}

void Session::login() {
    _pop3.connectToServer();
    std::cout << "[Session] Logging in to POP3 server\n";
    _pop3.login(_loginData.username(), _loginData.password());
    std::cout << "[Session] Logged in to POP3 server\n";

    _smtp.connectToServer();
    //_smtp.readResponse();
    //_smtp.sendCommand("EHLO localhost");
    //std::cout << _smtp.readResponse();
}