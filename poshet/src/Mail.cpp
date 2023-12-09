#include "Mail.hpp"

std::string MailInfo::from() const {
    return _from;
}
std::string MailInfo::subject() const {
    return _subject;
}