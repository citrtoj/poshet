#include "LoginData.hpp"

// LoginData::LoginData(const std::string& fullName, const std::string& username, const std::string& password, const std::string& pop3Domain, const std::string& smtpDomain) :
//     _fullName(fullName),
//     _username(username),
//     _password(password),
//     _pop3Domain(pop3Domain),
//     _smtpDomain(smtpDomain) {}

LoginData::LoginData(const std::vector<std::string>& data) {
    if (data.size() < LoginData::MIN_ARGS) {
        throw Exception("Not enough args for login data obj");
    }
    _fullName = data[0];
    _username = data[1];
    _password = data[2];
    _pop3Domain = data[3];
    _smtpDomain = data[4];
    std::cout << "[LoginData] built LoginData obj\n";
}

std::string LoginData::fullName() const {
    return _fullName;
}

std::string LoginData::username() const {
    return _username;
}

std::string LoginData::password() const {
    return _password;
}

std::string LoginData::pop3Domain() const {
    return _pop3Domain;
}

std::string LoginData::smtpDomain() const {
    return _smtpDomain;
}