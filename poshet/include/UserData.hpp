#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <array>

#include "Exceptions.hpp"

class UserData {
private:
    static std::array<std::string, 2> splitEmailAddress(const std::string& address);

    enum TextFields {
        FULLNAME,
        EMAILADDRESS,
        PASSWORD,
        POP3DOMAIN,
        POP3USERNAME,
        SMTPDOMAIN,
        SMTPUSERNAME
    };

    std::string _fullName,
                _emailAddress,
                _password,
                _pop3Domain,
                _pop3Username,
                _smtpDomain,
                _smtpUsername;

    int _pop3Port = -1, _smtpPort = -1;
    int _dbId = -1;

    bool _secure = false;
    bool _smtpAuthPlain = false;

public:
    static constexpr int MIN_ARGS = 5;
    
    UserData() {}
    UserData(const std::string& fullName, const std::string& emailAddress, const std::string& password, const std::string& pop3Domain, const std::string& pop3Username, const std::string& smtpDomain, bool secure, bool smtpAuthPlain, const std::string& smtpUsername, int pop3Port, int smtpPort);

    const std::string& fullName() const;
    const std::string& emailAddress() const;
    const std::string& password() const;
    const std::string& pop3Username() const;
    const std::string& pop3Domain() const;
    const std::string& smtpDomain() const;
    
    int dbId() const;
    void setDbId(int dbId);

    bool secure() const;
    bool smtpAuthPlain() const;
};