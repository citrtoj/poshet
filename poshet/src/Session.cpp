#include "Session.hpp"

// bool Session::_GMIME_INITIALIZED = false;

// void Session::_GMIME_INITIALIZE() {
//     if (!_GMIME_INITIALIZED) {
//         g_mime_init();
//         _GMIME_INITIALIZED = true;
//     }
// }

Session::Session(FileManager* manager) : _fileManager(manager) {
    // _GMIME_INITIALIZE();
    _db.setPath(_fileManager->databasePath());
}

Session::~Session() {
    // not its business to delete the file manager :)
}

void Session::setLoginData(const UserData& data) {
    _userData = data;
    _pop3.setHost(_userData.pop3Domain());
    _smtp.setHost(_userData.smtpDomain());
    std::cout << "[Session] Set data\n";
}

void Session::connectAndLoginToServers() {
    try {
        _pop3.connectToServer();
        _smtp.connectToServer();
        _pop3.login(_userData.pop3Username(), _userData.password());
        _db.addUser(_userData.emailAddress(), _userData.pop3Domain());
        _userData.setDbId(_db.getUser(_userData.emailAddress(), _userData.pop3Domain()));
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

void Session::getAllPop3AndSaveLocally() {
    if (_shouldRefreshConnection) {
        _pop3.resetConnection();
    }
    auto rawMail = _pop3.retrieveAllMail();
    for (const auto& rawMailData : rawMail) {
        auto mail = Mail(rawMailData.plainData);
        try {
            // info for db
            auto uidl = rawMailData.UIDL;
            auto id = mail.getHeaderField("Message-Id");
            auto hash = Utils::sha256(mail.plainText());
            auto fullMailId = id + "_" + hash;

            // save to fileManager, then to db
            _fileManager->saveMail(_userData.pop3Domain(), fullMailId, mail.plainText());
            // if this didn't fail, save all related info to db
            _db.addMail(fullMailId, _userData.dbId(), uidl);
        }
        catch (ServerException& e) {
            std::cout << "[Session] SERVER WARNING: " << e.what() << "\n";
        }
        catch(FileManagerException& e) {
            std::cout << "[Session] FILE MANAGER WARNING: " << e.what() << "\n";
        }
        catch (ConnectException& e) {
            throw;
        }
    }
    // temporary -- not sure how i'll handle this or whether i'll only set it at startup BUT...
    _shouldRefreshConnection = true;
}

std::vector<std::string> Session::getAllMailIdsFromDatabase() {
    return _db.getMailIdsOfUser(_userData.dbId());
}

void Session::loadMail(int count) {
    _mails.clear();
    getAllPop3AndSaveLocally();
    
    auto mailIds = getAllMailIdsFromDatabase();
    _mails.reserve(mailIds.size());
    for (auto mailId : mailIds) {
        auto x = _fileManager->getMail(_userData.pop3Domain(), mailId);
        _mails.push_back(Mail(x));
    }

}

const std::vector<Mail>& Session::retrieveMail(bool force) {
    if (force) {
        try {
            loadMail();
        }
        catch (Exception& e) {
            closeConnections();
            throw;
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

void Session::deleteMail(long idx) {
    // temporary, just to demonstrate DELE communication with the POP3 server
    if (idx < 0) {
        throw Exception("Invalid mail index to delete");
    }
    _pop3.markMailForDeletion(idx + 1);
}