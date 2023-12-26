#include "Mail.hpp"

Mail::Mail(const std::string& plainTextData) : _plainText(plainTextData) {
    _type = Type::FROM_PLAINTEXT;   
    parsePlainText();
}

void Mail::parsePlainText() {
    GMimeParser* parser;
    GMimeStream* stream;
    stream = g_mime_stream_mem_new_with_buffer(_plainText.c_str(), _plainText.length()); 
    parser = g_mime_parser_new_with_stream (stream);
    g_object_unref (stream);
    _message = g_mime_parser_construct_message (parser, NULL);
    g_object_unref (parser);
    _isMimeMessageInit = true;
}

Mail::Mail(const Mail& rhs) {
    std::cout << "Copy ctor\n";
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
    std::cout << "Move ctor\n";
    _type = rhs._type;
    _plainText = rhs._plainText;
    _message = rhs._message;
    rhs._message = nullptr;
    _isMimeMessageInit = rhs._isMimeMessageInit;
    rhs._isMimeMessageInit = false;
}

Mail::~Mail() {
	if (_isMimeMessageInit) {
        g_object_unref (_message);
    }
}

std::string Mail::getHeaderField(const std::string& key) const {
    if (!_isMimeMessageInit) {
        throw MailException("Mail not parsed yet");
    }
    const char *headerValue = g_mime_object_get_header((GMimeObject*)_message, key.c_str());
    if (headerValue == nullptr) {
        throw MailException("Requested mail lacks requested header");
    }
    return std::string(headerValue);
}

std::string Mail::plainText() const {
    return _plainText;
}