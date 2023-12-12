#include "SmtpConnection.hpp"

SMTPConnection::SMTPConnection() {}

SMTPConnection::SMTPConnection(const std::string& host) {
    setHost(host);
}

void SMTPConnection::setClientDomain(const std::string& domain) {
    _clientDomain = domain;
}

