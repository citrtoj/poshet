#pragma once

#include <string>
#include <wx/wx.h>
#include <iostream>

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
public:
    AppController(wxApp* app);

    void onCloseAnyWindow() override;

    void login();

    void onLoginSubmit() {
        login();
    }
    void onNewMail() {}
    void onReplyMail() {}
};