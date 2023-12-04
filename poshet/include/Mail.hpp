#pragma once
#include <string>
#include <vector>
#include <utility>

class Mail {
    std::string _plainText;
    std::vector<std::pair<std::string, std::string>> _attachments;
    std::vector<std::pair<std::string, std::string>> _headers;
};