#include "DatabaseConnection.hpp"

DatabaseConnection::DatabaseConnection() {}

DatabaseConnection::DatabaseConnection(const std::string& dbPath) : _path(dbPath) {
    _isPathSet = true;
    init();
}

void DatabaseConnection::setPath(const std::string& dbPath) {
    _path = dbPath;
    _isPathSet = true;
    init();
}

void DatabaseConnection::openDb() {
    int code = sqlite3_open(_path.c_str(), &_db);
    if (code) {
        throw DatabaseException("Could not open database");
    }
    _isDbOpened = true;
    initTables();
}

void DatabaseConnection::initTables() {
    std::string mailsTableQuery("CREATE TABLE IF NOT EXISTS mails ( mail_id TEXT PRIMARY KEY, mailbox_id INTEGER, content TEXT );");
    int code = sqlite3_exec(_db, mailsTableQuery.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException("Could not create databases!");
    }
}

void DatabaseConnection::init() {
    openDb();
    initTables();
}