#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <array>

#include "Exceptions.hpp"

class LoginData {
private:
    static std::array<std::string, 2> splitEmailAddress(const std::string& address);

    std::string _fullName, _emailAddress, _password, _pop3Domain, _pop3Username, _smtpDomain;
public:
    static constexpr int MIN_ARGS = 5;
    
    LoginData() {}
    LoginData(const std::vector<std::string>& data);
    const std::string& fullName() const;
    const std::string& emailAddress() const;
    const std::string& password() const;
    const std::string& pop3Username() const;
    const std::string& pop3Domain() const;
    const std::string& smtpDomain() const;
};