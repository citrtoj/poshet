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
    std::string mailsTableQuery("CREATE TABLE IF NOT EXISTS users (user_id TEXT PRIMARY KEY NOT NULL, mail_address TEXT, incoming_server TEXT, UNIQUE (mail_address, incoming_server));");
    int code = sqlite3_exec(_db, mailsTableQuery.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException("Could not create database table: users");
    }
    mailsTableQuery = "CREATE TABLE IF NOT EXISTS received_mail (mail_id TEXT PRIMARY KEY, user_id TEXT NOT NULL, tag TEXT, uidl TEXT, timestamp INTEGER NOT NULL, pop3_timestamp INTEGER NOT NULL, FOREIGN KEY(user_id) REFERENCES users(user_id));" ;
    code = sqlite3_exec(_db, mailsTableQuery.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException("Could not create database table: received_mail");
    }
    mailsTableQuery = "CREATE TABLE IF NOT EXISTS sent_mail (mail_id TEXT PRIMARY KEY, user_id INTEGER, tag TEXT, timestamp INTEGER NOT NULL, FOREIGN KEY(user_id) REFERENCES users(user_id));" ;
    code = sqlite3_exec(_db, mailsTableQuery.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException("Could not create database table: sent_mail");
    }
}

void DatabaseConnection::init() {
    openDb();
    initTables();
}

void DatabaseConnection::addUser(const std::string& mailAddress, const std::string& incomingServer) {
    auto userId = incomingServer + "://" + mailAddress;
    std::string query = "INSERT OR IGNORE INTO users (user_id, mail_address, incoming_server) VALUES ('"  + userId + "', '" + mailAddress + "', '" + incomingServer + "');";
    int code = sqlite3_exec(_db, query.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException("Could not add user to database");
    }
}

std::string DatabaseConnection::getUser(const std::string& mailAddress, const std::string& incomingServer) {
    std::string result;
    std::string query = "SELECT user_id FROM users WHERE mail_address like '" + mailAddress + "' AND incoming_server like '" + incomingServer + "';";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(_db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        throw DatabaseException(std::string("Can't prepare statement: ") + sqlite3_errmsg(_db));
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        result = userId;
    }
    sqlite3_finalize(stmt);
    return result;
}

void DatabaseConnection::addReceivedMail(const std::string& mailId, const std::string& userId, const std::string& uidl, unsigned long long date, const std::string tag) {
    // momentan presupunem ca toate serverele au UIDL
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    unsigned long long receivedTimestamp = static_cast<unsigned long long>(currentTime_t);
    std::string query = "INSERT OR IGNORE INTO received_mail (mail_id, user_id, tag, uidl, timestamp, pop3_timestamp) VALUES"
                        "('" + mailId
                        + "', '" + userId
                        + "', " +  (tag.empty() ? "NULL":  "'" + tag + "'")
                        + ", '" + uidl +
                        "', " + std::to_string(date) +
                        ", " + std::to_string(receivedTimestamp) + ");";
    int code = sqlite3_exec(_db, query.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException(std::string("Could not add mail to database: ") + sqlite3_errmsg(_db));
    }
}

std::vector<std::string> DatabaseConnection::getReceivedMailIdsOfUser(const std::string& id, const std::string& tag) {
    std::vector<std::string> result;
    std::string query = "SELECT mail_id FROM received_mail m join users u on m.user_id = u.user_id WHERE m.user_id = '" + id + "' ";

    if (!tag.empty()) {
        query += " AND m.tag like '" + tag + "'";
    }

    query += " ORDER BY timestamp DESC;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(_db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        throw DatabaseException(std::string("Can't prepare statement: ") + sqlite3_errmsg(_db));
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto mailId = sqlite3_column_text(stmt, 0);
        result.push_back((const char*)(mailId));
    }
    sqlite3_finalize(stmt);
    return result;
}

void DatabaseConnection::tagReceivedMail(const std::string& mailId, const std::string& tag) {
    std::string query = "UPDATE received_mail set tag='" + tag + "' where mail_id = '" + mailId + "';";
    int code = sqlite3_exec(_db, query.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException(std::string("Could not add mail to database: ") + sqlite3_errmsg(_db));
    }
}