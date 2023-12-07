#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

class Mail {
protected:
    std::string _plainText;
    std::unordered_map<std::string, std::string> _attachments;
    std::unordered_map<std::string, std::string> _headers;
public:
    Mail();
    std::string dump();
    bool attach(const std::string& filePath);
    std::string& plainText();
};

class MailSummary {
    std::string _to;
    std::string _from;
};