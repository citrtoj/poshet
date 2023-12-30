#include "Session.hpp"

Session::Session(MailFileManager* manager) : _fileManager(manager) {
    _db.setPath(_fileManager->databasePath());
}

Session::~Session() {
    // not its business to delete the file manager :)
}

void Session::setLoginData(const UserData& data) {
    _userData = data;
    _pop3.setHost(_userData.pop3Domain());
    _pop3.setSSL(_userData.pop3SSL());
    _smtp.setSSL(_userData.smtpSSL());
    _pop3.setPort(_userData.pop3Port());
    _smtp.setHost(_userData.smtpDomain());
    _smtp.setPort(_userData.smtpPort());
    std::cout << "[Session] Set data\n";
}

void Session::connectAndLoginToServers() {
    try {
        _pop3.connectToServer();
        _smtp.connectToServer();
        _pop3.login(_userData.pop3Username(), _userData.password());
        if (_userData.smtpAuth()) {
            _smtp.login(_userData.pop3Username(), _userData.password());
        }
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
    //TODO: SEND MAIL OH MY GOD...
    // Mail mail(to, _loginData.emailAddress(), subject, rawBody);
    // _smtp.sendMail(mail);

    // _shouldRefresh = true;
}

void Session::saveOnePop3MailLocally(size_t index, size_t byteSize) {
    std::string uidl = "";
    try {
        uidl = _pop3.retrieveOneMailUIDL(index);
    }
    catch(ServerResponseException& e) {
        // UIDL not supported by server
    }

    auto plainData = _pop3.retrieveOneMail(index, byteSize);

    Mail mail(plainData, ""); // this should be temporary shouldn't it 
    auto id = mail.getHeaderField("Message-Id");
    auto date = mail.getHeaderField("Date");
    auto hash = Utils::encodeToSHA256(plainData);
    auto fullMailId = id + "_" + hash;

    // save to fileManager, then to db
    _fileManager->saveMail(_userData.pop3Domain(), MailFileManager::MailType::RECEIVED, fullMailId, mail.plainText());
    // if this didn't fail, save all related info to db

    _db.addReceivedMail(fullMailId, _userData.dbId(), uidl, static_cast<unsigned long long>(Utils::mailDateToUnixTimestamp(date)));
}

void Session::getAllPop3AndSaveLocally() {
    if (_shouldRefreshConnection) {
        _pop3.resetConnection();
    }
    auto rawMail = _pop3.retrieveAllMailMetadata();
    for (const auto& rawMailData : rawMail) {
        try {
            saveOnePop3MailLocally(rawMailData.index, rawMailData.byteSize);
            // mark mail for deletion
            _pop3.markMailForDeletion(rawMailData.index);
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
    _pop3.resetConnection(); // to commit changes
    // todo: figure out how to use this bool properly
    _shouldRefreshConnection = true;
}

std::vector<DBMailData> Session::getAllMailFromDatabase() {
    return _db.getReceivedMailOfUser(_userData.dbId());
}

void Session::loadMail(int count) {
    _isMailCacheDirty = true;
    _mails.clear();
    
    auto mail = getAllMailFromDatabase();
    _mails.reserve(mail.size());
    for (auto mailData : mail) {
        auto x = _fileManager->getMail(_userData.pop3Domain(), MailFileManager::MailType::RECEIVED, mailData._mailId);
        _mails.push_back(Mail(x, mailData._mailId, mailData._mailTag));
    }
}

std::vector<std::string> Session::mailTags() const {
    return _db.getMailTags();
}

const std::vector<const Mail*>& Session::retrieveMail(const std::string& tag, bool forceReload) {
    if (forceReload) {
        try {
            getAllPop3AndSaveLocally();
            loadMail();
            // clearPop3Mailbox();
        }
        catch (Exception& e) {
            throw;
        }
    }
    if (tag == _currentTag and !_isMailCacheDirty) {
        return _mailsFilterCache;
    }

    _mailsFilterCache.clear();
    for (const auto& mail : _mails) {
        if (mail.tag() == tag) {
            _mailsFilterCache.push_back(&mail);
        }
    }
    _currentTag = tag;
    _isMailCacheDirty = false;
    return _mailsFilterCache;
}

const std::vector<const Mail*>& Session::retrieveAllMail(bool forceReload) {
    return retrieveMail("", forceReload);
}

const Mail& Session::getMailAt(size_t idx) {
    return *(_mailsFilterCache[idx]);
}

void Session::deleteMail(size_t idx) {
    _db.deleteMail(_mailsFilterCache[idx]->mailId());

    getAllPop3AndSaveLocally();
    loadMail();
    _isMailCacheDirty = true;
}