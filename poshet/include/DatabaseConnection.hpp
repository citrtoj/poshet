#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <vector>
#include <array>

#include <sqlite3.h>

#include "Utils.hpp"
#include "Exceptions.hpp"

struct DBMailData {
    std::string _mailId, _mailTag, _mailFilename;
};

class DatabaseConnection {
protected:
    bool _isPathSet = false;
    std::string _path;

    void initTables();

    bool _isDbOpened = false;
    sqlite3* _db;
    void openDb();
public:
    DatabaseConnection();
    DatabaseConnection(const std::string& dbPath);
    ~DatabaseConnection();

    void setPath(const std::string& dbPath);
    void init(); // opens db, creates tables if they don't exist

    void addUser(const std::string& mailAddress, const std::string& incomingServer);
    std::string getUser(const std::string& mailAddress, const std::string& incomingServer);

    void addReceivedMail(const std::string& mailId, const std::string& userId, const std::string& uidl, unsigned long long date, const std::string tag = "");
    std::vector<DBMailData> getReceivedMailOfUser(const std::string& id, const std::string& tag = "");

    std::string getFileNameOf(const std::string mailId);

    void tagReceivedMail(const std::string& mailId, const std::string& tag);

    std::vector<std::string> getMailTags(const std::string& userId) const;

    void deleteMail(const std::string& mailId);
};

class DatabaseException : public Exception {
public:
    DatabaseException(const std::string& message) : Exception(message) {}
};