#include "UserData.hpp"

std::array<std::string, 2> UserData::splitEmailAddress(const std::string& address) {
    size_t at = address.find('@');
    if (at == std::string::npos or at == 0 or at == address.length() - 1) {
        throw Exception("Could not derive username and domain from email address; most likely invalid");
    }
    return {
        address.substr(0, at),
        address.substr(at + 1)
    };
}

UserData::UserData(const std::string& fullName, const std::string& emailAddress, const std::string& password, const std::string& pop3Domain, const std::string& pop3Username, const std::string& smtpDomain, bool secure, bool smtpAuthPlain, const std::string& smtpUsername, int pop3Port, int smtpPort) {
    _fullName = fullName;
    _emailAddress = emailAddress;
    auto splitAddress = splitEmailAddress(_emailAddress);
    _password = password;
    _pop3Domain = pop3Domain.empty() ? splitAddress[1] : pop3Domain;
    _pop3Username = pop3Username.empty() ? _emailAddress : pop3Username;
    _smtpDomain = smtpDomain.empty() ? splitAddress[1] : smtpDomain;
    _smtpUsername = smtpUsername.empty() ? _emailAddress : smtpUsername;

    _smtpAuthPlain = smtpAuthPlain;
    _secure = secure;

    _pop3Port = pop3Port;
    _smtpPort = smtpPort;

    std::cout << "[UserData] built UserData obj\n";
}

const std::string& UserData::fullName() const {
    return _fullName;
}

const std::string& UserData::emailAddress() const {
    return _emailAddress;
}

const std::string& UserData::password() const {
    return _password;
}

const std::string& UserData::pop3Username() const {
    return _pop3Username;
}

const std::string& UserData::pop3Domain() const {
    return _pop3Domain;
}

const std::string& UserData::smtpDomain() const {
    return _smtpDomain;
}

void UserData::setDbId(int dbId) {
    _dbId = dbId;
}

int UserData::dbId() const {
    return _dbId;
}

bool UserData::secure() const {
    return _secure;
}

bool UserData::smtpAuthPlain() const {
    return _smtpAuthPlain;
}