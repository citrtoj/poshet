#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

class Mail {  //might replace with a MIME parser in the future
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

class MailInfo {
protected:
    //temp
    std::string _from;
    std::string _subject;
public:
    MailInfo(const Mail& mail) {
        //TEMP
        _from = "User "; // + std::to_string(COUNT);
        _subject = "Subject "; //+ std::to_string(COUNT);
        //COUNT++;
    }
    std::string from() const;
    std::string subject() const;
};