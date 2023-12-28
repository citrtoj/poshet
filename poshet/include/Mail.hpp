#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <utility>
#include <fstream>

#include <vmime/vmime.hpp>

#include "Utils.hpp"
#include "Exceptions.hpp"


class Mail {
protected:
    enum ConstructType {
        FROM_PLAINTEXT,
        FROM_USER_INPUT,
    };

    ConstructType _type;

    std::string _plainText;

    vmime::shared_ptr<vmime::message> _message;
    vmime::messageParser* _messageParser;
    
    void parsePlainText();

public:
    bool _isMimeMessageInit;
    Mail(const std::string& plainTextData);
    Mail(const Mail& rhs);
    Mail(Mail&& rhs);
    ~Mail();

    const std::string& plainText() const;
    
    std::string getHeaderField(const std::string& key, bool unicode = false) const;

    std::string getHTMLPart() const;
    std::string getPlainTextPart() const;
};


class MailException : public Exception {
public:
    MailException(const std::string& message) : Exception(message) {}
};