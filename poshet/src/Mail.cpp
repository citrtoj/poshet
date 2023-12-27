#include "Mail.hpp"

Mail::Mail(const std::string& plainTextData) : _plainText(plainTextData) {
    _type = Type::FROM_PLAINTEXT;   
    parsePlainText();
}

void Mail::parsePlainText() {
    _message = new vmime::message;
    _message->parse(_plainText);
    _isMimeMessageInit = true;
}

Mail::Mail(const Mail& rhs) {
    std::cout << "[Mail] Copy ctor\n";
    _type = rhs._type;
    if (_type == FROM_PLAINTEXT) {
        _plainText = rhs._plainText;
        parsePlainText();
    }
    else {
        throw MailException("Unsupported");
    }
}

Mail::Mail(Mail&& rhs) {
    std::cout << "[Mail] Move ctor\n";
    _type = rhs._type;
    _plainText = rhs._plainText;
    _message = rhs._message;
    rhs._message = nullptr;
    _isMimeMessageInit = rhs._isMimeMessageInit;
    rhs._isMimeMessageInit = false;
}

Mail::~Mail() {
    delete _message;
}

std::string Mail::getHeaderField(const std::string& key) const {
    if (!_isMimeMessageInit) {
        throw MailException("Mail not parsed yet");
    }
    if (_message->getHeader()->hasField(key)) {
        return _message->getHeader()->getField(key)->getValue()->generate();
    }
    else {
        return ""; //temporary
    }
    
}

const std::string& Mail::plainText() const {
    return _plainText;
}