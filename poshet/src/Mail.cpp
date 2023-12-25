#include "Mail.hpp"

Mail::Mail(const std::string& plainTextData) : _plainText(plainTextData) {
    _type = Type::FROM_PLAINTEXT;

    // std::istringstream i(plainTextData);
    // _mimeEntity = new MimeEntity(i);
}

Mail::Mail(const Mail& rhs) {
    _type = rhs._type;
    if (_type == FROM_PLAINTEXT) {
        _plainText = rhs._plainText;
        // std::istringstream i(_plainText);
        // _mimeEntity = new MimeEntity(i);
    }
    else {
        // not the most intelligent thing ever but idk if I can directly clone the entity
        // std::stringstream x;
        // x << *(rhs._mimeEntity);
        // _plainText = x.str();
        // _mimeEntity = new MimeEntity(x);
    }
}

Mail::Mail(Mail&& rhs) : _plainText(std::move(rhs._plainText)) {
    _type = rhs._type;
    // _mimeEntity = rhs._mimeEntity;
    // rhs._mimeEntity = nullptr;
}

Mail::~Mail() {
    // delete _mimeEntity;
}

std::string Mail::getHeaderField(const std::string& key) const {
    // if (!_mimeEntity->hasField(key)) {
    //     throw MailException("Requested mail lacks requested header");
    // }
    // return _mimeEntity->header().field(key).value();
    return "N/A";
}

std::string Mail::plainText() const {
    return _plainText;
    // return _mimeEntity->body();
}