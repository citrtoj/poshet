#pragma once

#include <string>
#include <exception>

class Exception : public std::exception {
protected:
    std::string _message;
public:
    Exception(const std::string& message) : _message(message) {}
    const char* what () {
        return _message.c_str();
    }
};

class ServerException : public Exception {
public:
    ServerException(const std::string& message) : Exception(message) {}
    const char* what () {
        return _message.c_str();
    }
};

class MailException : public Exception {
public:
    MailException(const std::string& message) : Exception(message) {}
    const char* what () {
        return _message.c_str();
    }
};

class FileManagerException : public Exception {
public:
    FileManagerException(const std::string& message) : Exception(message) {}
    const char* what () {
        return _message.c_str();
    }
};

class DatabaseException : public Exception {
public:
    DatabaseException(const std::string& message) : Exception(message) {}
    const char* what () {
        return _message.c_str();
    }
};