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