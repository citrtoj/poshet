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

    _pop3.resetConnection(); // pop3 won't see mail that was sent between start and end of connection

    auto rawMail = _pop3.retrieveAllMail();

    for (const auto& x : rawMail) {
        Mail tempMail = Mail(x.plainData);  // todo: implement move operator
        auto it = tempMail.headers().find("Message-Id");
        if (it != tempMail.headers().cend()) {
            auto it2 = _mailIds.find(it->second);
            if (it2 == _mailIds.cend()) {
                // mail not already in unordered set. add to unordered set, add to mail vector
                _mailIds.emplace(it->second);
                _mails.push_back(tempMail);
            }
        }
    }
    return _mails;
}

const Mail& Session::getMail(long idx) {
    if (idx < 0 or idx >= _mails.size()) {
        throw Exception("Invalid mail index");
    }

    return _mails[idx];
}