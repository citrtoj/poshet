#pragma once

#include "Pop3Connection.hpp"
#include "DatabaseConnection.hpp"
#include "Config.hpp"
#include "LoginData.hpp"
#include "Mail.hpp"
#include <string>

class Session {
protected:
    Pop3Connection _pop3;
    LoginData _loginData;

    std::vector<Mail> _mails;
    size_t _currentMail;
public:
    Session() {}
    void setLoginData(const LoginData& data);
    void login();

    ~Session() {
        std::cout << "[Session] dtor\n";
    }
};