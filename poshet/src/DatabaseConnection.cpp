#include "DatabaseConnection.hpp"

DatabaseConnection::DatabaseConnection() {}

DatabaseConnection::DatabaseConnection(const std::string& dbPath) : _path(dbPath) {
    _isPathSet = true;
    init();
}

DatabaseConnection::~DatabaseConnection() {
    if (_db != nullptr) {
        sqlite3_close(_db);
    }
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
    std::string mailsTableQuery("CREATE TABLE IF NOT EXISTS "
    "users (user_id TEXT PRIMARY KEY NOT NULL, "
           "mail_address TEXT, "
           "full_name TEXT, "
           "password TEXT, "
           "pop3_ssl BOOL, "
           "incoming_server TEXT, "
           "incoming_port TEXT, "
           "pop3_username TEXT, "
           "smtp_ssl BOOL, "
           "smtp_domain TEXT, "
           "smtp_port TEXT, "
           "smtp_auth BOOL, "
           "smtp_user TEXT, "
           "UNIQUE (user_id));");
    int code = sqlite3_exec(_db, mailsTableQuery.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException("Could not create database table: users");
    }
    mailsTableQuery = "CREATE TABLE IF NOT EXISTS received_mail (mail_id TEXT PRIMARY KEY NOT NULL, user_id TEXT NOT NULL, tag TEXT, uidl TEXT, timestamp INTEGER NOT NULL, pop3_timestamp INTEGER NOT NULL, FOREIGN KEY(user_id) REFERENCES users(user_id));" ;
    code = sqlite3_exec(_db, mailsTableQuery.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException("Could not create database table: received_mail");
    }
}

void DatabaseConnection::init() {
    openDb();
    initTables();
}

void DatabaseConnection::addUser(const UserData& data, bool replace) {
    // create quoted and hex-encoded database-ready stuff
    auto stringHexEncode = [](const std::string& input) {
        return "'" + Utils::hexEncode(input) + "'";
    };

    auto emailAddress = stringHexEncode(data.emailAddress());
    auto fullName = stringHexEncode(data.fullName());
    auto password = stringHexEncode(data.password());

    auto incomingServer = stringHexEncode(data.pop3Domain());
    auto incomingPort = stringHexEncode(data.pop3Port());
    auto incomingUsername = stringHexEncode(data.pop3Username());

    auto smtpDomain = stringHexEncode(data.smtpDomain());
    auto smtpPort = stringHexEncode(data.smtpPort());
    auto smtpUsername = stringHexEncode(data.smtpUsername());

    auto userId = "'" + (replace ? data.dbId() : Utils::generateUUID())  + "'";

    std::string queryPrefix = "INSERT OR REPLACE INTO users";
    std::string queryBody = "(user_id, mail_address, full_name, password, pop3_ssl, incoming_server, incoming_port, pop3_username, smtp_ssl, smtp_domain, smtp_port, smtp_auth, smtp_user) VALUES ("
    + userId + ", "
    + emailAddress + ", "
    + fullName + ", "
    + password + ", "
    + std::to_string(data.pop3SSL()) + ", "
    + incomingServer + ", "
    + incomingPort + ", "
    + incomingUsername + ", "
    + std::to_string(data.smtpSSL()) + ", "
    + smtpDomain + ", "
    + smtpPort + ", "
    + std::to_string(data.smtpAuth()) + ", "
    + smtpUsername
    + ");";
    std::string query = queryPrefix + queryBody;

    int code = sqlite3_exec(_db, query.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException(std::string("Could not add user to database: ") + sqlite3_errmsg(_db));
    }
}

int DatabaseConnection::userDataCallback(void* userData, int columnCount, char** columnValues, char** columnNames) {
    UserData& result = *reinterpret_cast<UserData*>(userData);

    result._dbId = columnValues[0];
    result._emailAddress = Utils::hexDecode(columnValues[1]);
    result._fullName = Utils::hexDecode(columnValues[2]);
    result._password = Utils::hexDecode(columnValues[3]);
    result._pop3SSL = std::stoi(columnValues[4]);
    result._pop3Domain = Utils::hexDecode(columnValues[5]);
    result._pop3Port = Utils::hexDecode(columnValues[6]);
    result._pop3Username = Utils::hexDecode(columnValues[7]);
    result._smtpSSL = std::stoi(columnValues[8]);
    result._smtpDomain = Utils::hexDecode(columnValues[9]);
    result._smtpPort = Utils::hexDecode(columnValues[10]);
    result._smtpAuth = std::stoi(columnValues[11]);
    result._smtpUsername = Utils::hexDecode(columnValues[12]);
    return 0;
}

UserData DatabaseConnection::getUserData(const std::string& userId) {
    std::string query = "SELECT * FROM users WHERE user_id like '" + userId + "';";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(_db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        throw DatabaseException(std::string("Can't prepare statement: ") + sqlite3_errmsg(_db));
    }
    UserData result;

    bool anyResults = false;
    int resultCode;
    while ((resultCode = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char* userId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        result._dbId = userId;

        const char* mailAddress = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        result._emailAddress = Utils::hexDecode(mailAddress);

        const char* fullName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        result._fullName = Utils::hexDecode(fullName);

        const char* password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        result._password = Utils::hexDecode(password);

        int pop3SSL = reinterpret_cast<int>(sqlite3_column_int(stmt, 4));
        result._pop3SSL = pop3SSL;

        const char* incomingServer = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        result._pop3Domain = Utils::hexDecode(incomingServer);

        const char* incomingPort = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        result._pop3Port = Utils::hexDecode(incomingPort);

        const char* pop3Username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        result._pop3Username = Utils::hexDecode(pop3Username);

        int smtpSSL = reinterpret_cast<int>(sqlite3_column_int(stmt, 8));
        result._smtpSSL = smtpSSL;

        const char* smtpDomain = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        result._smtpDomain = Utils::hexDecode(smtpDomain);

        const char* smtpPort = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        result._smtpPort = Utils::hexDecode(smtpPort);

        int smtpAuth = reinterpret_cast<int>(sqlite3_column_int(stmt, 11));
        result._smtpAuth = smtpAuth;

        const char* smtpUser = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        result._smtpUsername = Utils::hexDecode(smtpUser);

        break;
    }

    if (resultCode != SQLITE_DONE) {
        throw DatabaseException(std::string("SQLite error: ") + sqlite3_errmsg(_db));
    }
    if (!anyResults) {
        throw DatabaseConnection("No user with requested UUID");
    }
    sqlite3_finalize(stmt);
    return result;
}

int DatabaseConnection::allUsersDataCallback(void* userData, int columnCount, char** columnValues, char** columnNames) {
    std::vector<UserData>& allUsersData = *reinterpret_cast<std::vector<UserData>*>(userData);
    UserData user;

    user._dbId = columnValues[0];
    user._emailAddress = Utils::hexDecode(columnValues[1]);
    user._fullName = Utils::hexDecode(columnValues[2]);
    user._password = Utils::hexDecode(columnValues[3]);
    user._pop3SSL = std::stoi(columnValues[4]);
    user._pop3Domain = Utils::hexDecode(columnValues[5]);
    user._pop3Port = Utils::hexDecode(columnValues[6]);
    user._pop3Username = Utils::hexDecode(columnValues[7]);
    user._smtpSSL = std::stoi(columnValues[8]);
    user._smtpDomain = Utils::hexDecode(columnValues[9]);
    user._smtpPort = Utils::hexDecode(columnValues[10]);
    user._smtpAuth = std::stoi(columnValues[11]);
    user._smtpUsername = Utils::hexDecode(columnValues[12]);

    allUsersData.push_back(user);
    return 0; 
}

std::vector<UserData> DatabaseConnection::getAllUsersData() {
    std::vector<UserData> allUsersData;
    std::string query = "SELECT * FROM users;";
    if (sqlite3_exec(_db, query.c_str(), this->allUsersDataCallback, &allUsersData, nullptr) != SQLITE_OK) {
        throw DatabaseException(std::string("SQLite error: ") + sqlite3_errmsg(_db));
    }

    return allUsersData;
}

std::vector<std::string> DatabaseConnection::getAllUserIds() {
    std::vector<std::string> userIDs;

    std::string query = "SELECT user_id FROM users;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(_db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        throw DatabaseException(std::string("Can't prepare statement: ") + sqlite3_errmsg(_db));
    }
    int resultCode;
    while ((resultCode = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char* userID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        userIDs.push_back(userID);
    }
    if (resultCode != SQLITE_DONE) {
        throw DatabaseException(std::string("SQLite error: ") + sqlite3_errmsg(_db));
    }
    sqlite3_finalize(stmt);
    return userIDs;
}

std::string DatabaseConnection::getUserId(const std::string& mailAddress, const std::string& incomingServer) {
    std::string result;

    auto encodedEmailAddress = Utils::hexEncode(mailAddress);
    auto encodedIncomingServer = Utils::hexEncode(incomingServer);

    std::string query = "SELECT user_id FROM users WHERE mail_address like '" + encodedEmailAddress + "' AND incoming_server like '" + encodedIncomingServer + "';";
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

DBMailData DatabaseConnection::getMailInfo(const std::string& mailId) {
    DBMailData result;
    std::string query = "SELECT mail_id, tag FROM received_mail m WHERE m.mail_id = '" + mailId + "' ";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(_db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        throw DatabaseException(std::string("Can't prepare statement: ") + sqlite3_errmsg(_db));
    }
    bool any = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        any = true;
        auto mailId = sqlite3_column_text(stmt, 0);
        auto mailTag = sqlite3_column_text(stmt, 1);
        
        result = {
            (const char*)(mailId),
            mailTag != nullptr ? (const char*) mailTag : "",
            (const char*)(mailId)
        };
    }
    if (!any) {
        throw DatabaseException("No mail found with given index");
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<DBMailData> DatabaseConnection::getReceivedMailOfUser(const std::string& id, const std::string& tag) {
    std::vector<DBMailData> result;
    std::string query = "SELECT mail_id, tag FROM received_mail m join users u on m.user_id = u.user_id WHERE m.user_id = '" + id + "' ";

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
        auto mailTag = sqlite3_column_text(stmt, 1);
        
        result.push_back({
            (const char*)(mailId),
            mailTag != nullptr ? (const char*) mailTag : "",
            (const char*)(mailId)
        });
    }
    sqlite3_finalize(stmt);
    return result;
}

std::string DatabaseConnection::getFileNameOf(const std::string mailId) {
    // in my implementation the mailId itself is the filename but in case I want to change it at some point...
    return mailId;
}

void DatabaseConnection::tagReceivedMail(const std::string& mailId, const std::string& tag) {
    std::string query = "UPDATE received_mail set tag='" + tag + "' where mail_id = '" + mailId + "';";
    int code = sqlite3_exec(_db, query.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException(std::string("Could not add mail to database: ") + sqlite3_errmsg(_db));
    }
}

std::vector<std::string> DatabaseConnection::getMailTags(const std::string& userId) const {
    std::vector<std::string> result;
    std::string query = "SELECT DISTINCT tag from received_mail where user_id like '" + userId + "';";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(_db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        throw DatabaseException(std::string("Can't prepare statement: ") + sqlite3_errmsg(_db));
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        auto sqliteData = sqlite3_column_text(stmt, 0);
        if (sqliteData == nullptr) {
            result.push_back(std::string());
            continue;
        }
        std::string tag((const char*)sqliteData);
        if (!tag.empty()) {
            result.push_back(tag);
        }
    }
    sqlite3_finalize(stmt);
    return result;
}


void DatabaseConnection::deleteMail(const std::string& mailId) {
    std::string query = "DELETE FROM received_mail where mail_id = '" + mailId + "';";
    int code = sqlite3_exec(_db, query.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException(std::string("Could not delete mail from database: ") + sqlite3_errmsg(_db));
    }
}

void DatabaseConnection::deleteUser(const std::string& userId) {
    std::string query = "DELETE FROM users where user_id = '" + userId + "';";
    int code = sqlite3_exec(_db, query.c_str(), nullptr, nullptr, nullptr);
    if (code) {
        throw DatabaseException(std::string("Could not delete user from database: ") + sqlite3_errmsg(_db));
    }
}