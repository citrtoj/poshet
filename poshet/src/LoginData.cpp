#include "LoginData.hpp"

// LoginData::LoginData(const std::string& fullName, const std::string& username, const std::string& password, const std::string& pop3Domain, const std::string& smtpDomain) :
//     _fullName(fullName),
//     _username(username),
//     _password(password),
//     _pop3Domain(pop3Domain),
//     _smtpDomain(smtpDomain) {}

std::array<std::string, 2> LoginData::splitEmailAddress(const std::string& address) {
    size_t at = address.find('@');
    if (at == std::string::npos or at == 0 or at == address.length() - 1) {
        throw Exception("Could not derive username and domain from email address; most likely invalid");
    }
    return {
        address.substr(0, at),
        address.substr(at + 1)
    };
}

LoginData::LoginData(const std::vector<std::string>& data) {
    if (data.size() < LoginData::MIN_ARGS) {
        throw Exception("Not enough args for login data object");
    }
    _fullName = data[0];
    _emailAddress = data[1];
    auto splitAddress = splitEmailAddress(_emailAddress);
    _password = data[2];
    _pop3Domain = data[3].empty() ? splitAddress[1] : data[3];
    _pop3Username = data[4].empty() ? splitAddress[0] : data[4];
    _smtpDomain = data[5].empty() ? splitAddress[1] : data[5];
    std::cout << "[LoginData] built LoginData obj\n";
}

const std::string& LoginData::fullName() const {
    return _fullName;
}

const std::string& LoginData::emailAddress() const {
    return _emailAddress;
}

const std::string& LoginData::password() const {
    return _password;
}

const std::string& LoginData::pop3Username() const {
    return _pop3Username;
}

const std::string& LoginData::pop3Domain() const {
    return _pop3Domain;
}

const std::string& LoginData::smtpDomain() const {
    return _smtpDomain;
}