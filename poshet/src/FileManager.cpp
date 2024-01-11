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
    _rootPath = joinToFullPath(_root, _location);
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

void FileManager::saveFile(const std::string& filePath, const std::string& rawData, bool overwrite) {
    if (access(filePath.c_str(), F_OK) != -1 and !overwrite) {
        throw FileExistsException();
    }
    std::ofstream outputFile(filePath, std::ios::out | std::ios::binary);
    outputFile << rawData;
}

void FileManager::saveMail(const std::string& mailboxName, const std::string& mailFilename, const std::string& rawMailData) {
    // all the filemanager does is save files and get files, not its business to handle how to generate the filenames and everything
    auto mailboxPath = mailbox(mailboxName);
    auto filePath = joinToFullPath(mailboxPath, mailFilename);
    try {
        saveFile(filePath, rawMailData);
    }
    catch (FileExistsException& e) {
        // if file exists and has the exact same name then it's okay... we hope nobody has messed with our data
        return;
    }
}

std::string FileManager::getMail(const std::string& mailboxName, const std::string& mailFilename) {
    auto mailboxPath = mailbox(mailboxName);
    auto filePath = joinToFullPath(mailboxPath, mailFilename);
    if (access(filePath.c_str(), F_OK) == -1) {
        throw FileManagerException("File " + filePath + " is not valid");
    }
    return readFile(filePath);
}

void FileManager::saveAttachment(const std::string& filePath, const std::string& rawData, bool overwrite) {
    try {
        saveFile(filePath, rawData, overwrite);
    }
    catch(FileExistsException& e) {
        throw FileManagerException("Another file already exists at the same location (overwriting was not permitted)");
    }
}

std::string FileManager::readFile(const std::string& filePath) {
    try {
        std::ifstream inputFile(filePath, std::ios::in | std::ios::binary);
        std::stringstream buffer;
        buffer << inputFile.rdbuf();
        std::string result = buffer.str();
        return result;
    }
    catch(...) {
        throw FileManagerException("Error opening file " + filePath);
    }
}

void FileManager::deleteFile(const std::string& filePath) {
    if (remove(filePath.c_str()) != 0) {
        throw FileManagerException("Error deleting file");
    }
}

void FileManager::deleteMail(const std::string& mailboxName, const std::string& mailFilename) {
    auto mailboxPath = mailbox(mailboxName);
    auto filePath = joinToFullPath(mailboxPath, mailFilename);
    if (access(filePath.c_str(), F_OK) == -1) {
        return;
    }
    deleteFile(filePath);
}