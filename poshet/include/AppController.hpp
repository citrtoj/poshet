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
    Session* _session;
    LoginFrame* _loginFrame;
    DashboardFrame* _dashboardFrame;
    MailCreatorFrame* _mailCreatorFrame;
    wxApp* _mainApp;
    size_t _selectedMail;

    bool _isMailCreatorOpen = false;

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
    void onMailSend() override {}
    void onCloseMailCreator() override {
        _mailCreatorFrame->notifyToClose();
        // todo: perhaps think of a better way to singletonify this class
        _isMailCreatorOpen = false;
    }
};