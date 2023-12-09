#pragma once

#include "Pop3Connection.hpp"
#include "DatabaseConnection.hpp"
#include "Config.hpp"
#include "LoginData.hpp"
#include <string>

class Session {
protected:
    Pop3Connection _pop3;
    LoginData _loginData;
public:
    Session() {}
    void setLoginData(const LoginData& data);
    void login();
    ~Session() {
        std::cout << "[Session] dtor\n";
    }
};