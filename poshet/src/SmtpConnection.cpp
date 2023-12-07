#include "SmtpConnection.hpp"

SMTPConnection::SMTPConnection() {}

SMTPConnection::SMTPConnection(const std::string& host) : _host(host) {
    _hostSet = true;
}

void SMTPConnection::setHost(const std::string& host) {
    _host = host;
    _hostSet = true;
}