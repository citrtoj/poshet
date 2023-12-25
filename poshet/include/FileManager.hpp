#pragma once

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include <vector>
#include <string>
#include <iostream>

#include "Exceptions.hpp"

class FileManager {
private:
    static std::string joinToFullPath(const std::string& root, const std::string& addon);
    static void createFolder(const std::string& path);

    std::string _root = "$HOME";
    std::string _location = ".poshet";
    std::string _dbName = "mail.db";
    std::string _mailFolderName = "mail";

    std::string _rootPath;
    bool _isRootPathInit = false;
    void initRootPath();

    void checkIfRootPathInit();

    bool _isFolderInit = false;
    void initFolder();

    bool _isDbInit = false;
    void initDatabase();

    bool _isMailFolderInit = false;
    void initMailFolder();

    void init();
public:
    FileManager();
    FileManager(const std::string& rootLocation);

    void setRootLocation(const std::string& rootLocation);
    
    std::string databasePath() const;
};