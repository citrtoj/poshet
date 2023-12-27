#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Exceptions.hpp"
#include "Pop3Connection.hpp"
#include "SmtpConnection.hpp"
#include "DatabaseConnection.hpp"
#include "Config.hpp"
#include "FileManager.hpp"
#include "UserData.hpp"
#include "Mail.hpp"
#include "Utils.hpp"

class Session {
protected:
    // static bool _GMIME_INITIALIZED;
    // static void _GMIME_INITIALIZE();

    POP3Connection _pop3;
    SMTPConnection _smtp;
    DatabaseConnection _db;

    FileManager* _fileManager;
    UserData _userData;

    std::vector<Mail> _mails;
    int _currentMail;
    std::string _currentTag;
    
    bool _shouldRefreshConnection = false;
    bool _shouldRepopulateMail = true;

    void loadMail(int count = -1);

    void getAllPop3AndSaveLocally(); // might either do this only once, at startup, or offer a Refresh button
    
    std::vector<std::string> getAllMailIdsFromDatabase();
    void dropUIDLFromPop3(/* list of UIDLs marked as deleted */);

public:
    Session(FileManager* manager);
    ~Session();
    void setLoginData(const UserData& data);
    void connectAndLoginToServers();

    void closeConnections();
    void resetConnections();

    const std::vector<Mail>& retrieveMail(bool force = false);
    const Mail& getMail(long idx);
    void deleteMail(long idx);

    void sendMail(const std::string& to, const std::string& subject, const std::string& rawBody);
};