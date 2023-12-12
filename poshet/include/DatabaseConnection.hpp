#pragma once
#include <string>

class DatabaseConnection {
protected:
    
public:
    DatabaseConnection(const std::string& dbName);
    void initTables();
};