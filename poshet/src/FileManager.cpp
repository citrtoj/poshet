#include "FileManager.hpp"

FileManager::FileManager() {
    init();
}

FileManager::FileManager(const std::string& rootLocation) : _root(rootLocation) {
    init();
}

std::string FileManager::joinToFullPath(const std::string& root, const std::string& addon) {
    if (addon.empty()) {
        return root;
    }
    return root + "/" + addon;
}

void FileManager::createFolderOrCheckIfExists(const std::string& path) {
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

void FileManager::initRootPath() {
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

void FileManager::initFolder() {
    createFolderOrCheckIfExists(_rootPath);
}

std::string FileManager::databasePath() const {
    return joinToFullPath(_rootPath, _dbName);
}

void FileManager::initDatabase() {
    int dbFD = open(databasePath().c_str(), O_RDWR | O_CREAT | O_APPEND, PERMISSIONS);
    if (dbFD == -1) {
        throw FileManagerException("Error creating database file"); 
    }
    close(dbFD);
}

void FileManager::initMailFolder() {
    auto mailFolderPath = joinToFullPath(_rootPath, _mailFolderName);

    createFolderOrCheckIfExists(mailFolderPath);
}

void FileManager::init() {
    initRootPath();
    initFolder();
    initDatabase();
    initMailFolder();
}

void FileManager::setRootLocation(const std::string& rootLocation) {
    // this will not get rid of the previous location, rather, it will populate the newly chosen folder (if it can) with the appropriate folders. it will not copy the info either
    _root = rootLocation;
    init();
}

std::string FileManager::mailbox(const std::string& mailboxName) {
    // creates mailbox if it can, returns mailbox path
    auto mailboxPath = joinToFullPath(joinToFullPath(_rootPath, _mailFolderName), mailboxName);
    createFolderOrCheckIfExists(mailboxPath);
    return mailboxPath;
}

// since we're storing raw ASCII data I think string is fine
void FileManager::saveMail(const std::string& mailboxName, const std::string& mailFilename, const std::string& rawMailData) {
    // all the filemanager does is save files and get files
    // it does NOT care about sha256 stuff
    auto mailboxPath = mailbox(mailboxName);
    auto filePath = joinToFullPath(mailboxPath, mailFilename);
    if (access(filePath.c_str(), F_OK) != -1) {
        return;
    }
    int mailFD = open(filePath.c_str(), O_CREAT | O_RDWR, PERMISSIONS);
    if (mailFD == -1) {
        throw FileManagerException("Error creating file " + filePath);
    }
    size_t len = rawMailData.length();
    int writeCode = Utils::writeLoop(mailFD, rawMailData.c_str(), len * sizeof(char));
    close(mailFD);
}

std::string FileManager::getMail(const std::string& mailboxName, const std::string& mailFilename) {
    auto mailboxPath = mailbox(mailboxName);
    auto filePath = joinToFullPath(mailboxPath, mailFilename);
    if (access(filePath.c_str(), F_OK) == -1) {
        throw FileManagerException("File " + filePath + " is not valid");
    }
    int mailFD = open(filePath.c_str(), O_RDWR);
    if (mailFD == -1) {
        throw FileManagerException("Error opening file " + filePath);
    }
    // citim in buffere de 512 at a time ca mna, si adaugam la un std::string ca sa nu fie chiar void*

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
        std::cout << "readcode:" << readSoFar << " " << readCode << "\n";
        result += buffer;
    }
    while (readCode == BUFFER_SIZE); 
    return result;
}