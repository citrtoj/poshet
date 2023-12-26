#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <array>

#include "Exceptions.hpp"

class UserData {
private:
    static std::array<std::string, 2> splitEmailAddress(const std::string& address);

    std::string _fullName,
                _emailAddress,
                _password,
                _pop3Domain,
                _pop3Username,
                _smtpDomain;

    int _dbId = -1;

    bool _secure = false;

public:
    static constexpr int MIN_ARGS = 5;
    
    UserData() {}
    UserData(const std::vector<std::string>& data);
    const std::string& fullName() const;
    const std::string& emailAddress() const;
    const std::string& password() const;
    const std::string& pop3Username() const;
    const std::string& pop3Domain() const;
    const std::string& smtpDomain() const;
    
    int dbId() const;
    void setDbId(int dbId);
};