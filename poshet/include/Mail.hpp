#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <utility>
#include <sstream>

#include <gmime/gmime.h>

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

    GMimeMessage* _message;
    
    void parsePlainText();

public:
    bool _isMimeMessageInit;
    Mail(const std::string& plainTextData);
    Mail(const Mail& rhs);
    Mail(Mail&& rhs);
    ~Mail();

    std::string plainText() const;
    std::string getHeaderField(const std::string& key) const;
};
