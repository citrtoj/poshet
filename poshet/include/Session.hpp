#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Pop3Connection.hpp"
#include "DatabaseConnection.hpp"
#include "Config.hpp"
#include "LoginData.hpp"
#include "Mail.hpp"
#include "Utils.hpp"

class Session {
protected:
    Pop3Connection _pop3;
    LoginData _loginData;

    std::vector<Mail> _mails;
    //temporary solution to duplicate mail before I implement the databases part
    std::unordered_set<std::string> _mailIds;
    int _currentMail;
public:
    Session() {}
    void setLoginData(const LoginData& data);
    void login();

    const auto& retrieveMail() {
        auto rawMail = _pop3.retrieveAllMailHeaders();

        for (const auto& x : rawMail) {
            Mail tempMail = Mail(x.plainData);  // todo implement move op
            auto it = tempMail.headers().find("Message-Id");
            if (it != tempMail.headers().cend()) {
                auto it2 = _mailIds.find(it->second);
                if (it2 == _mailIds.cend()) {
                    // mail not already in unordered set. add to unordered set, add to mail vector
                    _mailIds.emplace(it->second);
                    _mails.push_back(tempMail);
                }
            }
        }
        return _mails;
    }

    ~Session() {
        std::cout << "[Session] dtor\n";
    }
};