#include "Session.hpp"

Session::Session(const LoginData& loginData) :
    _loginData(loginData),
    _pop3(_loginData.pop3Domain())
{
    std::cout << "session created\n";
}

void Session::login() {
    _pop3.connectToPop3Server(_loginData.username(), _loginData.password());
    std::cout << _pop3.execCommand("RETR");
}