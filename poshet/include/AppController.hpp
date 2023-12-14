#pragma once

#include <string>
#include <wx/wx.h>
#include <iostream>

#include "Mail.hpp"
#include "LoginFrame.hpp"
#include "DashboardFrame.hpp"
#include "FrameSubscribers.hpp"
#include "MailCreatorFrame.hpp"
#include "Session.hpp"
#include "Exceptions.hpp"

class AppController : public LoginFrameSubscriber, public DashboardFrameSubscriber, public MailCreatorFrameSubscriber {
protected:
    wxApp* _mainApp;
    Session* _session;
    LoginFrame* _loginFrame;
    DashboardFrame* _dashboardFrame;
    
    bool _isMailCreatorOpen = false;
    MailCreatorFrame* _mailCreatorFrame;
    size_t _selectedMail;


    // internal methods
    void login();
public:
    AppController(wxApp* app);

    // subscriber overrides
    void onCloseAnyWindow() override;
    void onLoginSubmit() override {
        login();
    }
    void onRefreshMailList() override {
        const auto& mails = _session->retrieveMail();
        _dashboardFrame->setMailList(mails);
    }
    void onNewMail() override;
    void onReplyMail() override {}
    void onMailCreatorSend() override;
    void onMailCreatorClose() override;
};