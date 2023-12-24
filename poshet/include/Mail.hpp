#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <utility>

#include <mimetic/mimetic.h>
using namespace mimetic;

#include "Utils.hpp"
#include "Exceptions.hpp"


class Mail {
protected:
    enum Type {
        FROM_PLAINTEXT,
        FROM_USER_INPUT,
    };
    Type _type;

    MimeEntity* _mimeEntity = nullptr;
    std::string _plainText;



    //void dumpToPlaintext();

public:
    Mail(const std::string& plainTextData);
    //Mail(const std::string& to, const std::string& from, const std::string& subject, const std::string& userInputBody);

    std::string plainText() const;
    std::string getHeaderField(const std::string& key) const;
};
