#pragma once
#include <string>
#include <vector>
#include <utility>

class Mail {
protected:
    std::string _plainText;
    std::vector<std::pair<std::string, std::string>> _attachments;
    std::vector<std::pair<std::string, std::string>> _headers;
public:
    Mail();
    std::string dump();
    bool attach(const std::string& filePath);
    std::string& plainText();
};