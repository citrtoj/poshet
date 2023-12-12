#pragma once

#include <string>
#include <wx/wx.h>
#include <iostream>

#include "Mail.hpp"
#include "LoginFrame.hpp"
#include "DashboardFrame.hpp"
#include "FrameSubscribers.hpp"
#include "NewMailDialog.hpp"
#include "Session.hpp"
#include "Exceptions.hpp"

class AppController : public LoginFrameSubscriber, public DashboardFrameSubscriber {
protected:
    Session* _session;
    LoginFrame* _loginFrame;
    DashboardFrame* _dashboardFrame;
    wxApp* _mainApp;
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
    void onNewMail() {}
    void onReplyMail() {}
};