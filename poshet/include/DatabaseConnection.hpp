#pragma once

#include <string>

#include <sqlite3.h>

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
};