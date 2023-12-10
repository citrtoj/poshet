#include "Mail.hpp"

std::string MailInfo::from() const {
    return _from;
}
std::string MailInfo::subject() const {
    return _subject;
}

void MailInfo::setFrom(const std::string& from) {
    _from = from;
}
void MailInfo::setSubject(const std::string& subject) {
    _subject = subject;
}