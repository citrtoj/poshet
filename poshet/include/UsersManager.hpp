#pragma once

#include "FileManager.hpp"
#include "DatabaseConnection.hpp"
#include "Session.hpp"

class UsersManager {
public:
    UsersManager(FileManager* manager);

    const std::vector<UserData>& users();
    void addUser(const UserData& data, bool replace = false);

    void deleteUser(const UserData& data);

protected:
    void getUsers();

    FileManager* _fileManager;
    DatabaseConnection db;

    std::vector<UserData> _users;
};