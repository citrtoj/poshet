#include "Session.hpp"

bool Session::_GMIME_INITIALIZED = false;

void Session::_GMIME_INITIALIZE() {
    if (!_GMIME_INITIALIZED) {
        g_mime_init();
        _GMIME_INITIALIZED = true;
    }
}

Session::Session(FileManager* manager) : _fileManager(manager) {
    _GMIME_INITIALIZE();
    _db.setPath(_fileManager->databasePath());
}

Session::~Session() {
    // not its business to delete the file manager :)
}

void Session::setLoginData(const std::vector<std::string> data) {
    std::cout << "[Session] Setting login data\n";
    _userData = UserData(data);
    std::cout << "[Session] Setting POP3 domain\n";
    _pop3.setHost(_userData.pop3Domain());
    _smtp.setHost(_userData.smtpDomain());
    std::cout << "[Session] Set login data\n";
}

void Session::connectAndLoginToServers() {
    try {
        _pop3.connectToServer();
        _smtp.connectToServer();
        _pop3.login(_userData.pop3Username(), _userData.password());
        _db.addUser(_userData.emailAddress(), _userData.pop3Domain());
        _userData.setDbId(_db.getUser(_userData.emailAddress(), _userData.pop3Domain()));
        refreshMail();
    }
    catch (Exception& e) {
        closeConnections();
        throw;
    }
}

void Session::closeConnections() {
    try {
        _pop3.quitConnection();
    }
    catch(Exception& e) {
        _pop3.closeConnection();
    }
    _smtp.closeConnection();
}

void Session::resetConnections() {
    closeConnections();
    connectAndLoginToServers();
}

void Session::sendMail(const std::string& to, const std::string& subject, const std::string& rawBody) {
    // Mail mail(to, _loginData.emailAddress(), subject, rawBody);
    // _smtp.sendMail(mail);

    // _shouldRefresh = true;
}

void Session::refreshMail() {  // populates _mails (mail which will be shown to user) and handles all subsequent sub-operations: pop3 grabbing, db interaction...
    if (_shouldRefreshConnection) {
        _pop3.resetConnection();
    }
    auto rawMail = _pop3.retrieveAllMail();
    // get info from plain pop3 data... save mail locally with filemanager... 

    _mails.clear(); // we'll fix this
    _mails.reserve(rawMail.size());
    for (const auto& x : rawMail) {
        _mails.push_back(Mail(x.plainData));
    }

    // temporary -- not sure how i'll handle this or whether i'll only set it at startup BUT...
    _shouldRefreshConnection = true;
}

const std::vector<Mail>& Session::retrieveMail(bool force) {
    if (force) {
        refreshMail();
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
}