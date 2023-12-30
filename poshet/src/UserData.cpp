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

UserData::UserData(
    const std::string& emailAddress,
    const std::string& fullName,
    const std::string& password,

    bool pop3SSL,
    const std::string& pop3Domain,
    const std::string& pop3Port,
    const std::string& pop3Username,
    
    bool smtpSSL,
    const std::string& smtpDomain,
    const std::string& smtpPort,
    bool smtpAuth,
    const std::string& smtpUsername
) {
    _emailAddress = emailAddress;
    auto splitAddress = splitEmailAddress(_emailAddress);
    _fullName = fullName.empty() ? splitAddress[0] : fullName;
    _password = password;

    _pop3SSL = pop3SSL;
    _pop3Domain = pop3Domain.empty() ? splitAddress[1] : pop3Domain;
    _pop3Port = pop3Port.empty() ? std::to_string(POP3Connection::DefaultPort(pop3SSL)) : pop3Port;
    _pop3Username = pop3Username.empty() ? _emailAddress : pop3Username;

    _smtpSSL = smtpSSL;
    _smtpDomain = smtpDomain.empty() ? splitAddress[1] : smtpDomain;
    _smtpPort = smtpPort.empty() ? std::to_string(SMTPConnection::DefaultPort(smtpSSL)) : smtpPort;
    _smtpAuth = smtpAuth;
    _smtpUsername = smtpUsername.empty() ? _emailAddress : smtpUsername;

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

void UserData::setDbId(const std::string& dbId) {
    _dbId = dbId;
}

const std::string& UserData::dbId() const {
    return _dbId;
}
