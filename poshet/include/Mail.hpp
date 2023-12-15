#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <sstream>

#include <iostream>

#include "Utils.hpp"

class Mail {
protected:
    enum Type {
        FROM_PLAINTEXT,
        FROM_USER_INPUT,
    };
    Type _type;

    std::string _plainText;

    std::string _userInputBody;
    //std::unordered_map<std::string, std::string> _attachments;
    std::unordered_map<std::string, std::string> _headers;

    void dumpToPlaintext();

public:
    Mail(const std::string& plainTextData);
    Mail(const std::string& to, const std::string& from, const std::string& subject, const std::string& userInputBody);

    std::string getHeader(const std::string& key) const;
    const std::string& plainText() const;
    const auto& headers() const;

    static std::unordered_map<std::string, std::string> mailHeaders(const std::string& input);
};
