#include "Session.hpp"

// --- subscriber-specific ---

void Session::subscribe(SessionObserver* observer) {
    _observer = observer;
}

void Session::notifyObserver() {
    _observer->handleSessionDataUpdate();
}

// --- ctor ---

Session::Session(FileManager* manager) : _fileManager(manager) {
    _db.setPath(_fileManager->databasePath());
}

// --- internal methods ---

void Session::saveOnePop3MailLocally(size_t index, size_t byteSize) {
    std::string uidl = "";
    try {
        uidl = _pop3.retrieveOneMailUIDL(index);
    }
    catch(ServerResponseException& e) {
        // UIDL not supported by server
    }

    auto plainData = _pop3.retrieveOneMail(index, byteSize);
    Mail mail(plainData, ""); // temporary mail instance
    auto id = mail.getHeaderField("Message-Id");
    auto date = mail.getHeaderField("Date");
    auto timestamp = Utils::mailDateToUnixTimestamp(date);
    if (timestamp == -1) {
        throw Exception("Could not derive timestamp of mail");
    }
    auto hash = Utils::encodeToSHA256(plainData);
    auto fullMailId = id + "_" + hash;

    // save to fileManager, then to db
    // if this didn't fail, save all related info to db
    _db.addReceivedMail(fullMailId, _userData.dbId(), uidl, static_cast<unsigned long long>(timestamp));
    auto filename = _db.getFileNameOf(fullMailId);
    _fileManager->saveMail(_userData.pop3Domain(), FileManager::MailType::RECEIVED, filename, mail.plainText());
}

void Session::getAllPop3AndSaveLocally(bool deleteOnSave) {
    if (_shouldRefreshConnection) {
        _pop3.resetConnection();
    }
    
    auto rawMail = _pop3.retrieveAllMailMetadata();
    bool hasAnyBeenDeleted = false;
    for (const auto& rawMailData : rawMail) {
        try {
            saveOnePop3MailLocally(rawMailData.index, rawMailData.byteSize);
            if (deleteOnSave) {
                hasAnyBeenDeleted = true;
                _pop3.markMailForDeletion(rawMailData.index);
            }
        }
        catch (ServerException& e) {
            std::cout << "[Session] SERVER WARNING: " << e.what() << "\n";
            return;
        }
        catch(FileManagerException& e) {
            std::cout << "[Session] FILE MANAGER WARNING: " << e.what() << "\n";
            return;
        }
        catch (ConnectException& e) {
            throw;
        }
    }
    if (hasAnyBeenDeleted) {
        _pop3.resetConnection();
    }
    _shouldRefreshConnection = true;
}

std::vector<DBMailData> Session::getAllMailFromDatabase() {
    return _db.getReceivedMailOfUser(_userData.dbId());
}

// --- public methods ---

void Session::setLoginData(const UserData& data) {
    _userData = data;
    std::cout << "[Session] Set data\n";
}

void Session::connectAndLoginToServers() {
    try {
        _pop3.setHost(_userData.pop3Domain());
        _pop3.setSSL(_userData.pop3SSL());
        _pop3.setPort(_userData.pop3Port());

        _pop3.connectToServer();
        _pop3.login(_userData.pop3Username(), _userData.password());

        auto smtp = SMTPConnection();
        initSMTP(smtp);
        // if it worked, then the login is valid; break
        smtp.closeConnection();

        _db.addUser(_userData.emailAddress(), _userData.pop3Domain());
        _userData.setDbId(_db.getUser(_userData.emailAddress(), _userData.pop3Domain()));
    }
    catch (Exception& e) {
        throw;
    }
}

void Session::initSMTP(SMTPConnection& smtp) {
    smtp.setSSL(_userData.smtpSSL());
    smtp.setHost(_userData.smtpDomain());
    smtp.setPort(_userData.smtpPort());

    smtp.connectToServer();
    if (_userData.smtpAuth()) {
        smtp.login(_userData.pop3Username(), _userData.password());
    }
}

void Session::closeConnections() {
    try {
        _pop3.quitConnection();
    }
    catch(Exception& e) {
        _pop3.closeConnection();
    }
}

void Session::resetConnections() {
    closeConnections();
    connectAndLoginToServers();
}

void Session::sendMail(const std::string& from, const std::string& to, const std::string& rawBody) {
    auto smtp = SMTPConnection();
    initSMTP(smtp);
    smtp.sendMail(from, to, rawBody);
    smtp.closeConnection();

    _isMailCacheDirty = true;
    _observer->handleSessionDataUpdate();
}

void Session::reloadMailFromDatabase() {
    _isMailCacheDirty = true;
    _mails.clear();
    
    auto mail = getAllMailFromDatabase();
    _mails.reserve(mail.size());
    for (auto mailData : mail) {
        auto x = _fileManager->getMail(_userData.pop3Domain(), FileManager::MailType::RECEIVED, mailData._mailFilename);
        _mails.push_back(Mail(x, mailData._mailId, mailData._mailTag));
    }
}

std::vector<std::string> Session::mailTags() const {
    return _db.getMailTags(_userData.dbId());
}

const std::vector<const Mail*>& Session::retrieveMail(const std::string& tag, bool forceReload) {
    if (forceReload) {
        try {
            getAllPop3AndSaveLocally();
            reloadMailFromDatabase();
        }
        catch (Exception& e) {
            throw;
        }
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
    if (forceReload) {
        try {
            getAllPop3AndSaveLocally();
            reloadMailFromDatabase();
        }
        catch (Exception& e) {
            throw;
        }
    }

    _mailsFilterCache.clear();
    for (const auto& mail : _mails) {
        _mailsFilterCache.push_back(&mail);
    }
    _currentTag = "";
    _isMailCacheDirty = false;
    return _mailsFilterCache;
}

const Mail& Session::getMailAt(ssize_t idx) {
    if (idx < 0) {
        throw Exception("No mail at given index");
    }
    return *(_mailsFilterCache[idx]);
}

void Session::deleteMail(ssize_t idx) {
    if (idx < 0) {
        throw Exception("No mail at given index");
    }

    
    _db.deleteMail(_mailsFilterCache[idx]->mailId());
    reloadMailFromDatabase();
    _observer->handleSessionDataUpdate();
}

void Session::tagMail(ssize_t idx, const std::string& userInput) {
    if (idx < 0) {
        throw Exception("No mail at given index");
    }
    _db.tagReceivedMail(_mailsFilterCache[idx]->mailId(), userInput);
    reloadMailFromDatabase();
    _observer->handleSessionDataUpdate();
}