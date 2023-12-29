#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <vector>

#include <sqlite3.h>

#include "Utils.hpp"
#include "Exceptions.hpp"

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

    void setPath(const std::string& dbPath);
    void init(); // opens db, creates tables if they don't exist

    void addUser(const std::string& mailAddress, const std::string& incomingServer);
    int getUser(const std::string& mailAddress, const std::string& incomingServer);

    void addReceivedMail(const std::string& mailId, int userId, const std::string& uidl, unsigned long long date, const std::string tag = "");
    std::vector<std::string> getReceivedMailIdsOfUser(int id);

    void tagReceivedMail(const std::string& mailId, const std::string& tag);

};

class DatabaseException : public Exception {
public:
    DatabaseException(const std::string& message) : Exception(message) {}
};