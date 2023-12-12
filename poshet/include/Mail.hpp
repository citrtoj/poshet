#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <sstream>

#include "Utils.hpp"

class Mail {
protected:
    std::string _plainText;
    //std::unordered_map<std::string, std::string> _attachments;
    std::unordered_map<std::string, std::string> _headers;
public:
    Mail() {}
    Mail(const std::string& plaintextData) : _plainText(plaintextData) {
        // todo: separate mail contents from mail text
        // this will all make much more sense when I actually use a MIME Parser for all of this
        _headers = mailHeaders(_plainText);
    }
    const std::string& plainText() const {
        return _plainText;
    }
    const auto& headers() const {
        return _headers;
    }

    std::unordered_map<std::string, std::string> mailHeaders(const std::string& input) {
        std::unordered_map<std::string, std::string> result;
        std::istringstream iss(input);
        std::string line;
        std::string lastKey;
        while (std::getline(iss, line)) {
            if (!line.empty() && (line[0] == '\t' or line[0] == ' ')) {
                result[lastKey] += ' ' + Utils::strip(line);
            }
            else {
                size_t colonPos = line.find(": ");
                if (colonPos != std::string::npos) {
                    lastKey = line.substr(0, colonPos);
                    std::string value = Utils::strip(line.substr(colonPos + 2, line.length()));
                    result[lastKey] = value;
                }
                else {
                    result[lastKey] += ' ' + Utils::strip(line);
                }
            }
        }

        return result;
    }

};