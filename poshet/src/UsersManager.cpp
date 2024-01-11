#include "UsersManager.hpp"

UsersManager::UsersManager(FileManager* manager):
    _fileManager(manager)
{
    db.setPath(_fileManager->databasePath());
    getUsers();
}

const std::vector<UserData>& UsersManager::users() {
    getUsers();
    return _users;
}

void UsersManager::getUsers() {
    _users = db.getAllUsersData();
}

void UsersManager::addUser(const UserData& data, bool replace) {
    Session session(_fileManager);
    session.setLoginData(data);
    session.connectAndLoginToServers();
    db.addUser(data, replace);
}

void UsersManager::deleteUser(const UserData& data) {
    // get list of filenames, remove files
    auto userMail = db.getReceivedMailOfUser(data.dbId());
    for (const auto& mail : userMail) {
        db.deleteMail(mail._mailId);
        _fileManager->deleteMail(data.pop3Domain(), mail._mailFilename);
        // filename delete file
    }
    db.deleteUser(data.dbId());
}