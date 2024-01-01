#pragma once

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>

#include "Exceptions.hpp"
#include "Utils.hpp"

class MailFileManager {
public:
    enum MailType {
        RECEIVED,
        SENT
    };

private:
    std::string _receivedFolder = "Received";
    std::string _sentFolder = "Sent";

    const std::unordered_map<MailType, std::string&> _typeFolderNames = {
        {MailType::RECEIVED, _receivedFolder},
        {MailType::SENT, _sentFolder}
    };
    
    // utility functions
    static std::string joinToFullPath(const std::string& root, const std::string& addon);
    static void createFolderOrCheckIfExists(const std::string& path);
    static constexpr int PERMISSIONS = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    
    std::string _root = ".";
    std::string _location = ".poshet";
    std::string _dbName = "mail.db";
    std::string _mailFolderName = "mail";
    std::string _rootPath;

    void initRootPath();
    void initFolder();
    void initDatabase();
    void initMailFolder();
    void init();

    std::string mailbox(const std::string& mailboxName);

    void saveFile(const std::string& filePath, const std::string& rawData, bool overwrite = false);

public:
    MailFileManager();
    MailFileManager(const std::string& rootLocation);
    void setRootLocation(const std::string& rootLocation);
    std::string databasePath() const;
    void saveMail(const std::string& mailboxName, MailType type, const std::string& mailFilename, const std::string& rawMailData);
    std::string getMail(const std::string& mailboxName, MailType type, const std::string& mailFilename);

    void saveAttachment(const std::string& filePath, const std::string& rawData, bool overwrite = false);

};

class FileManagerException : public Exception {
public:
    FileManagerException(const std::string& message = "") : Exception(message) {}
};

class FileExistsException : public FileManagerException {
public:
    FileExistsException(const std::string& message = "") : FileManagerException(message) {}
};