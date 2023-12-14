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
        closeConnections();
        throw;
    }
}

void Session::closeConnections() {
    _pop3.closeConnection();
    _smtp.closeConnection();
}

void Session::resetConnections() {
    closeConnections();
    connectAndLoginToServers();
}

void Session::sendMail(const std::string& to, const std::string& subject, const std::string& rawBody) {
    Mail mail(to, _loginData.emailAddress(), subject, rawBody);
    _smtp.sendMail(mail);
}

const std::vector<Mail>& Session::retrieveMail() {
    // clears vector, reads from POP3, populates vector
    _pop3.resetConnection(); 

    auto rawMail = _pop3.retrieveAllMail();

    _mails.clear();

    for (const auto& x : rawMail) {
        Mail tempMail = Mail(x.plainData);  // todo: implement move operator and all...
        _mails.push_back(tempMail);
    }
    return _mails;
}

const Mail& Session::getMail(long idx) {
    if (idx < 0 or idx >= _mails.size()) {
        throw Exception("Invalid mail index");
    }

    return _mails[idx];
}

void Session::deleteMail(long idx) {
    // temporary, just to demonstrate DELE communication with the POP3 server
    if (idx < 0) {
        throw Exception("Invalid mail index to delete");
    }
    _pop3.markMailForDeletion(idx + 1);
    _pop3.resetConnection(); // in order to "commit" the changes
}