#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Exceptions.hpp"

class LoginData {
private:
    std::string _fullName, _username, _password, _pop3Domain, _smtpDomain;
public:
    static constexpr int MIN_ARGS = 5;
    
    //LoginData(const std::string& fullName, const std::string& username, const std::string& password, const std::string& pop3Domain, const std::string& smtpDomain);
    LoginData(const std::vector<std::string>& data);
    std::string fullName() const;
    std::string username() const;
    std::string password() const;
    std::string pop3Domain() const;
    std::string smtpDomain() const;
};