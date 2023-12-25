#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Pop3Connection.hpp"
#include "SmtpConnection.hpp"
#include "DatabaseConnection.hpp"
#include "Config.hpp"
#include "FileManager.hpp"
#include "LoginData.hpp"
#include "Mail.hpp"
#include "Utils.hpp"

class Session {
protected:
    POP3Connection _pop3;
    SMTPConnection _smtp;
    DatabaseConnection _db;

    FileManager* _fileManager;
    LoginData _loginData;

    std::vector<Mail> _mails;
    int _currentMail;
    
    // for future use
    bool _shouldRefreshConnection = false;
    bool _shouldRepopulateMail = true;

    void refreshMail();

public:
    Session(FileManager* manager);
    ~Session();
    void setLoginData(const std::vector<std::string> data);
    void connectAndLoginToServers();

    void closeConnections();
    void resetConnections();

    const std::vector<Mail>& retrieveMail(bool force = false);
    const Mail& getMail(long idx);
    void deleteMail(long idx);

    void sendMail(const std::string& to, const std::string& subject, const std::string& rawBody);
};