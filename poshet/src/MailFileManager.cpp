#include "MailFileManager.hpp"

MailFileManager::MailFileManager() {
    init();
}

MailFileManager::MailFileManager(const std::string& rootLocation) : _root(rootLocation) {
    init();
}

std::string MailFileManager::joinToFullPath(const std::string& root, const std::string& addon) {
    if (addon.empty()) {
        return root;
    }
    return root + "/" + addon;
}

void MailFileManager::createFolderOrCheckIfExists(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            throw FileManagerException("Creating directory " + path + " failed");
        }
    }
    else if (! S_ISDIR(st.st_mode)) {
        throw FileManagerException(path + " is not a directory");
    }
}

void MailFileManager::initRootPath() {
    if (_root == "$HOME") {
        const char* homeDir = getenv("HOME");
        if (homeDir != nullptr) {
            _rootPath = joinToFullPath(std::string(homeDir), _location);
        }
        else {
            throw FileManagerException("Could not get home directory location");
        }
    }
    else {
        _rootPath = joinToFullPath(_root, _location);
    }
}

void MailFileManager::initFolder() {
    createFolderOrCheckIfExists(_rootPath);
}

std::string MailFileManager::databasePath() const {
    return joinToFullPath(_rootPath, _dbName);
}

void MailFileManager::initDatabase() {
    int dbFD = open(databasePath().c_str(), O_RDWR | O_CREAT | O_APPEND, PERMISSIONS);
    if (dbFD == -1) {
        throw FileManagerException("Error creating database file"); 
    }
    close(dbFD);
}

void MailFileManager::initMailFolder() {
    auto mailFolderPath = joinToFullPath(_rootPath, _mailFolderName);
    createFolderOrCheckIfExists(mailFolderPath);
}

void MailFileManager::init() {
    initRootPath();
    initFolder();
    initDatabase();
    initMailFolder();
}

void MailFileManager::setRootLocation(const std::string& rootLocation) {
    // this will not get rid of the previous location, rather, it will populate the newly chosen folder (if it can) with the appropriate folders. it will not copy the info either
    _root = rootLocation;
    init();
}

std::string MailFileManager::mailbox(const std::string& mailboxName) {
    // creates mailbox if it can, returns mailbox path
    auto mailboxPath = joinToFullPath(joinToFullPath(_rootPath, _mailFolderName), mailboxName);
    createFolderOrCheckIfExists(mailboxPath);
    // create Received and Sent folders
    auto mailboxSentPath = joinToFullPath(mailboxPath, _sentFolder);
    createFolderOrCheckIfExists(mailboxSentPath);
    auto mailboxReceivedPath = joinToFullPath(mailboxPath, _receivedFolder);
    createFolderOrCheckIfExists(mailboxReceivedPath);
    return mailboxPath;
}

void MailFileManager::saveMail(const std::string& mailboxName, MailFileManager::MailType type, const std::string& mailFilename, const std::string& rawMailData) {
    // all the filemanager does is save files and get files, not its business to handle how to generate the filenames and everything
    auto mailboxPath = mailbox(mailboxName);
    auto filePath = joinToFullPath(joinToFullPath(mailboxPath, _typeFolderNames.find(type)->second), mailFilename);
    if (access(filePath.c_str(), F_OK) != -1) {
        // todo: file exists. check SHA256; if not, continue
        std::cout << "file exists\n";
        return;
    }
    int mailFD = open(filePath.c_str(), O_CREAT | O_RDWR, PERMISSIONS);
    if (mailFD == -1) {
        throw FileManagerException("Error creating file " + filePath);
    }
    // truncate file
    size_t len = rawMailData.length();
    int writeCode = Utils::writeLoop(mailFD, rawMailData.c_str(), len * sizeof(char));
    close(mailFD);
}

std::string MailFileManager::getMail(const std::string& mailboxName, MailFileManager::MailType type, const std::string& mailFilename) {
    auto mailboxPath = mailbox(mailboxName);

    auto filePath = joinToFullPath(joinToFullPath(mailboxPath, _typeFolderNames.find(type)->second), mailFilename);
    if (access(filePath.c_str(), F_OK) == -1) {
        throw FileManagerException("File " + filePath + " is not valid");
    }

    int mailFD = open(filePath.c_str(), O_RDWR);
    if (mailFD == -1) {
        throw FileManagerException("Error opening file " + filePath);
    }
    // citim in buffere de 512 at a time ca mna
    constexpr int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE + 1];
    int readCode;
    size_t readSoFar = 0;
    std::string result;
    do {
        memset(buffer, 0, (BUFFER_SIZE + 1) * sizeof(char));
        readCode = Utils::readLoop(mailFD , buffer, BUFFER_SIZE * sizeof(char));
        // todo: error handling. if == 1...
        readSoFar += readCode;
        std::cout << "[FileManager] Readcode: " << readCode << "\n";
        result += buffer;
    }
    while (readCode == BUFFER_SIZE); 
    return result;
}