#include "Mail.hpp"

Mail::Mail(const std::string& plainTextData) : _plainText(plainTextData) {
    _type = Type::FROM_PLAINTEXT;   
    parsePlainText();
}

void Mail::parsePlainText() {
    _message = std::make_shared<vmime::message>();
    _message->parse(_plainText);
    _messageParser = new vmime::messageParser(_message);
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
    _messageParser = rhs._messageParser;
    rhs._messageParser = nullptr;
            // Transfer ownership of the shared_ptr
    _message = std::move(rhs._message);
    _isMimeMessageInit = rhs._isMimeMessageInit;
    rhs._isMimeMessageInit = false;
}

Mail::~Mail() {
    delete _messageParser;
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

std::string Mail::getHTML() const {
    for (size_t i = 0 ; i < _messageParser->getTextPartCount() ; ++i) {
        const vmime::textPart& part = *_messageParser->getTextPartAt(i);
        // text/html
        if (part.getType().getSubType() == vmime::mediaTypes::TEXT_HTML) {
            const vmime::htmlTextPart& hp = dynamic_cast<const vmime::htmlTextPart&>(part);
            // HTML text is in "hp.getText()"
            std::string htmlText;
            vmime::utility::outputStreamStringAdapter x(htmlText);
            hp.getText()->extract(x);
            return htmlText;
        }
    }
    throw MailException("Mail does not have HTML parts");
}