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
    std::string mailsTableQuery("CREATE TABLE IF NOT EXISTS users (user_id INTEGER PRIMARY KEY, mail_address TEXT, incoming_server TEXT, UNIQUE (mail_address, incoming_server));");
    int code = sqlite3_exec(_db, mailsTableQuery.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException("Could not create database table: users");
    }
    mailsTableQuery = "CREATE TABLE IF NOT EXISTS mail (mail_id INTEGER PRIMARY KEY, user_id INTEGER, tag TEXT, FOREIGN KEY(user_id) REFERENCES users(user_id));" ;
    code = sqlite3_exec(_db, mailsTableQuery.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException("Could not create database table: mail");
    }
}

void DatabaseConnection::init() {
    openDb();
    initTables();
}

void DatabaseConnection::addUser(const std::string& mailAddress, const std::string& incomingServer) {
    // todo: bobby tables
    std::string query = "INSERT OR IGNORE INTO users (mail_address, incoming_server) VALUES ('" + mailAddress + "', '" + incomingServer + "');";
    int code = sqlite3_exec(_db, query.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException("Could not add user to database");
    }
}