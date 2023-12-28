#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <array>

#include "Exceptions.hpp"
#include "Pop3Connection.hpp"
#include "SmtpConnection.hpp"

class UserData {
private:
    static std::array<std::string, 2> splitEmailAddress(const std::string& address);

    std::string _emailAddress,
                _fullName,
                _password;
    
    bool _pop3SSL = false;
    std::string _pop3Domain,
                _pop3Port,
                _pop3Username;
    
    bool _smtpSSL = false;
    std::string _smtpDomain,
                _smtpPort;
    bool _smtpAuth = false;
    std::string _smtpUsername;

    int _dbId = -1;

public:
    static constexpr int MIN_ARGS = 5;
    
    UserData() {}
    UserData(
        const std::string& emailAddress,
        const std::string& fullName,
        const std::string& password,

        bool pop3SSL,
        const std::string& pop3Domain,
        const std::string& pop3Port,
        const std::string& pop3Username,

        bool smtpSSL,
        const std::string& smtpDomain,
        const std::string& smtpPort,
        bool smtpAuth,
        const std::string& smtpUsername
    );

    const std::string& fullName() const;
    const std::string& emailAddress() const;
    const std::string& password() const;
    const std::string& pop3Username() const;
    const std::string& pop3Domain() const;
    const std::string& smtpDomain() const;
    
    int dbId() const;
    void setDbId(int dbId);
};

class ValidationException : public Exception {
public:
    ValidationException(const std::string& message) : Exception(message) {}
};