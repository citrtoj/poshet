#pragma once
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>

class DatabaseConnection {
protected:
    SQLite::Database _db;
public:
    DatabaseConnection(const std::string& dbName);
    void initTables();
};