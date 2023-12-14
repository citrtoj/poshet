#include "Mail.hpp"

Mail::Mail(const std::string& plainTextData): _plainText(plainTextData) {
    // todo: separate mail contents from mail text
    // this will all make much more sense when I actually use a MIME Parser for all of this
    _headers = mailHeaders(_plainText);
    _type = Type::FROM_PLAINTEXT;
}

Mail::Mail(const std::string& to, const std::string& from, const std::string& subject, const std::string& userInputBody) {
    // temporary until MIME parser
    _headers["To"] = "<" + to + ">";
    _headers["From"] = "<" + from + ">";
    _headers["Subject"] = subject;
    _userInputBody = userInputBody;
    _type = Type::FROM_USER_INPUT;

    dumpToPlaintext();
}

std::unordered_map<std::string, std::string> Mail::mailHeaders(const std::string& input) {
    //temporary function
    
    // only get everything up until crlfcrlf
    std::string trimmedInput;

    auto headerSplit = input.find("\r\n\r\n");
    if (headerSplit != std::string::npos) {
        trimmedInput = input.substr(0, headerSplit);
    }
    else {
        trimmedInput = input;
    }

    std::unordered_map<std::string, std::string> result;
    std::istringstream iss(trimmedInput);
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

void Mail::dumpToPlaintext() {
    if (_type != FROM_USER_INPUT) {
        return;
    }
    _plainText.clear();
    // VERY TEMPORARY: just adds headers to the user input and then dumps the user input
    for (const auto& [headerName, headerValue] : _headers) {
        _plainText += headerName + ": " + headerValue + "\r\n";
    }
    _plainText += "\r\n";
    _plainText += _userInputBody;
}

std::string Mail::getHeader(const std::string& key) const {
    auto it = _headers.find(key);
    if (it != _headers.cend()) {
        return it->second;
    }
    else {
        return "N/A";
    }
}