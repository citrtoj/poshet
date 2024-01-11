#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <vector>
#include <array>

#include <sqlite3.h>

#include "Utils.hpp"
#include "Exceptions.hpp"
#include "UserData.hpp"

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


    static int userDataCallback(void* userData, int columnCount, char** columnValues, char** columnNames);
    static int allUsersDataCallback(void* userData, int columnCount, char** columnValues, char** columnNames);

public:
    DatabaseConnection();
    DatabaseConnection(const std::string& dbPath);
    ~DatabaseConnection();

    void setPath(const std::string& dbPath);
    void init(); // opens db, creates tables if they don't exist

    void addUser(const UserData& data);
    UserData getUserData(const std::string& userId);
    std::vector<std::string> getAllUserIds();
    std::string getUserId(const std::string& mailAddress, const std::string& incomingServer);
    std::vector<UserData> getAllUsersData();

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