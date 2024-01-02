#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <array>

#include "Exceptions.hpp"
#include "Pop3Connection.hpp"
#include "SmtpConnection.hpp"
#include "DatabaseConnection.hpp"
#include "Config.hpp"
#include "FileManager.hpp"
#include "UserData.hpp"
#include "Mail.hpp"
#include "Utils.hpp"


class SessionObserver {
public:
    virtual void handleSessionDataUpdate() = 0;
};

class Session {

protected:
    SessionObserver* _observer;
    void notifyObserver();
public:
    void subscribe(SessionObserver* observer);

protected:
    SMTPConnection _smtp;
    POP3Connection _pop3;
    DatabaseConnection _db;

    FileManager* _fileManager;
    UserData _userData;

    std::vector<Mail> _mails;
    std::vector<const Mail*> _mailsFilterCache;
    bool _isMailCacheDirty = true;
    std::string _currentTag;
    
    bool _shouldRefreshConnection = false;
    bool _shouldRepopulateMail = true;


    void saveOnePop3MailLocally(size_t index, size_t byteSize);
    void getAllPop3AndSaveLocally(bool deleteOnSave = true);

    std::vector<DBMailData> getAllMailFromDatabase();
    
    void reloadMailFromDatabase();

public:
    Session(FileManager* manager);

    const std::string& fullName() const {
        return _userData.fullName();
    }

    const std::string& emailAddress() const {
        return _userData.emailAddress();
    }

    void setLoginData(const UserData& data);
    void connectAndLoginToServers();

    void closeConnections();
    void resetConnections();

    std::vector<std::string> mailTags() const;

    const std::vector<const Mail*>& retrieveMail(const std::string& tag = "", bool forceReload = false);
    const std::vector<const Mail*>& retrieveAllMail(bool forceReload = false);
    const Mail& getMailAt(ssize_t idx); // uses currently displayed mail
    void deleteMail(ssize_t idx);
    void tagMail(ssize_t idx, const std::string& userInput);

    void sendMail(const std::string& from, const std::string& to, const std::string& rawBody);
};