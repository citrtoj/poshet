#include "Session.hpp"

Session::Session(const LoginData& loginData) :
    _loginData(loginData),
    _pop3(loginData.pop3Domain())
{
}

void Session::login() {
    _pop3.connectToPop3Server();
    _pop3.login(_loginData.username(), _loginData.password());
}