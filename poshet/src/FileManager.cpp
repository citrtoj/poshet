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

void FileManager::createFolder(const std::string& path) {
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
    _isRootPathInit = true;
}

void FileManager::initFolder() {
    createFolder(_rootPath);
    _isFolderInit = true;
}

std::string FileManager::databasePath() const {
    return joinToFullPath(_rootPath, _dbName);
}

void FileManager::initDatabase() {
    int dbFD = open(databasePath().c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (dbFD == -1) {
        throw FileManagerException("Error creating database file"); 
    }
    close(dbFD);
    _isDbInit = true;
}

void FileManager::initMailFolder() {
    auto mailFolderPath = joinToFullPath(_rootPath, _mailFolderName);

    createFolder(mailFolderPath);
    _isMailFolderInit = true;
}

void FileManager::init() {
    initRootPath();
    initFolder();
    initDatabase();
    initMailFolder();
}

void FileManager::setRootLocation(const std::string& rootLocation) {
    _root = rootLocation;
    init();
}