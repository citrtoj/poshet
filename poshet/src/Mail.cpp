#include "Mail.hpp"

Mail::Mail(const std::string& plainTextData) : _plainText(plainTextData) {
    _type = Type::FROM_PLAINTEXT;   
    parsePlainText();
}

void Mail::parsePlainText() {
    
    
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
    // _message = rhs._message;
    // rhs._message = nullptr;
    _isMimeMessageInit = rhs._isMimeMessageInit;
    rhs._isMimeMessageInit = false;
}

Mail::~Mail() {
	if (_isMimeMessageInit) {
        // g_object_unref (_message);
    }
}

std::string Mail::getHeaderField(const std::string& key) const {
    if (!_isMimeMessageInit) {
        throw MailException("Mail not parsed yet");
    }
    return "N/A";
    // const char *headerValue = g_mime_object_get_header((GMimeObject*)_message, key.c_str());
    // if (headerValue == nullptr) {
    //     throw MailException("Requested mail lacks requested header");
    // }
    // return std::string(headerValue);
}

const std::string& Mail::plainText() const {
    return _plainText;
}