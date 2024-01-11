#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <array>

#include "Exceptions.hpp"
// for default port info
#include "Pop3Connection.hpp"
#include "SmtpConnection.hpp"

class UserData {
private:
    static std::array<std::string, 2> splitEmailAddress(const std::string& address);

   

public:
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

    std::string _dbId = "";
    
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
    const std::string& smtpUsername() const;

    bool pop3SSL() const {
        return _pop3SSL;
    }
    bool smtpSSL() const {
        return _smtpSSL;
    }

    const std::string& pop3Port() const {
        return _pop3Port;
    }
    const std::string& smtpPort() const {
        return _smtpPort;
    }
    
    bool smtpAuth() const {
        return _smtpAuth;
    }

    const std::string& dbId() const;
    void setDbId(const std::string& dbId);
};

class ValidationException : public Exception {
public:
    ValidationException(const std::string& message) : Exception(message) {}
};