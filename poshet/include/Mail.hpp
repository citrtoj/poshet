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
    Mail(const std::string& plaintextData) : _plainText(plaintextData) {}

    std::string to();
    std::string from();
    std::string dump();
    bool attach(const std::string& filePath);
    std::string& plainText();
};

class MailInfo {
protected:
    //temp
    std::string _from;
    std::string _subject;
public:
    MailInfo(const Mail& mail) {
        
    }
    std::string from() const;
    std::string subject() const;
};