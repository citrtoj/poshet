#pragma once

#include <string>
#include <iostream>

#include <wx/wx.h>

#include "Mail.hpp"
#include "LoginFrame.hpp"
#include "DashboardFrame.hpp"
#include "FrameSubscribers.hpp"
#include "MailCreatorFrame.hpp"
#include "Session.hpp"
#include "FileManager.hpp"
#include "Exceptions.hpp"

class AppController : public LoginFrameSubscriber, public DashboardFrameSubscriber, public MailCreatorFrameSubscriber {
protected:
    wxApp* _mainApp;
    Session* _session;

    // GUIs
    LoginFrame* _loginFrame;
    DashboardFrame* _dashboardFrame;
    bool _isMailCreatorOpen = false;
    MailCreatorFrame* _mailCreatorFrame;

    FileManager* _fileManager;
    
    size_t _selectedMail;

    // internal methods
    void login();
    void getSetMail(bool force = false);
    void warnUnimplemented();
public:
    AppController(wxApp* app);
    ~AppController();

    // subscriber overrides
    
    void onCloseApp() override;
    
    void onLoginSubmit() override;

    void onSelectMail() override;
    void onRefreshMailList() override;
    void onNewMail() override;
    void onReplyMail() override;
    void onForwardMail() override;
    void onDeleteMail() override;

    void onMailCreatorSend() override;
    void onMailCreatorClose() override;
};