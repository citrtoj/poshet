#pragma once
#include <string>


class LoginModel {
public:
    bool _submitted;

    std::string _fullname,
                _user,
                _pass,
                _smtpDomain,
                _pop3Domain
                ;
};