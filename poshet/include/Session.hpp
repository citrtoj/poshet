#pragma once

#include "Pop3Connection.hpp"
#include "DatabaseConnection.hpp"
#include "Config.hpp"
#include "LoginData.hpp"

class Session {
protected:
    Pop3Connection _pop3;
    LoginData _loginData;
public:
    Session(const LoginData& loginData);
    void login();
};