#include "DatabaseConnection.hpp"

DatabaseConnection::DatabaseConnection(const std::string& dbName) :
    _db(dbName, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE)
{}

void DatabaseConnection::initTables() {
    std::string mailsTableQuery("CREATE TABLE IF NOT EXISTS mails ( mail_id TEXT PRIMARY KEY, mailbox_id INTEGER, content TEXT );");
    _db.exec(mailsTableQuery);
}

