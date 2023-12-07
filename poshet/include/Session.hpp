#pragma once

#include "Pop3Connection.hpp"
#include "DatabaseConnection.hpp"
#include "Config.hpp"
#include "LoginData.hpp"

class Session {
protected:
    Pop3Connection _pop3;
    // SMTPConnection _smtp;
    LoginData _loginData;
    // todo: some sort of UserConfigData which I could perhaps CRUD in combo with the db.. such as "delete the mail after x days" and so on
public:
    Session(const LoginData& loginData);
    void login();
};