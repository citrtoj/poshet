#include "Session.hpp"

void Session::setLoginData(const std::vector<std::string> data) {
    std::cout << "[Session] Setting login data\n";
    _loginData = LoginData(data);
    std::cout << "[Session] Setting POP3 domain\n";
    _pop3.setHost(_loginData.pop3Domain());
    _smtp.setHost(_loginData.smtpDomain());
    std::cout << "[Session] Set login data\n";
}

void Session::connectAndLoginToServers() {
    try {
        _pop3.connectToServer();
        _smtp.connectToServer();
        _pop3.login(_loginData.pop3Username(), _loginData.password());
    }
    catch (Exception& e) {
        _pop3.closeConnection();
        _smtp.closeConnection();
        throw;
    }
}

void Session::sendMail(const std::string& to, const std::string& subject, const std::string& rawBody) {
    Mail mail(to, _loginData.emailAddress(), subject, rawBody);
    _smtp.sendMail(mail);
}