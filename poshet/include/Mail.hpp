#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <utility>
#include <sstream>

#include <vmime/vmime.hpp>

#include "Utils.hpp"
#include "Exceptions.hpp"


class Mail {
protected:
    enum Type {
        FROM_PLAINTEXT,
        FROM_USER_INPUT,
    };
    Type _type;

    std::string _plainText;

    vmime::message* _message;
    
    void parsePlainText();

public:
    bool _isMimeMessageInit;
    Mail(const std::string& plainTextData);
    Mail(const Mail& rhs);
    Mail(Mail&& rhs);
    ~Mail();

    const std::string& plainText() const;
    std::string getHeaderField(const std::string& key) const;
};
