#pragma once

#include <string>
#include <iostream>

#include <wx/wx.h>

#include "Mail.hpp"
#include "LoginFrame.hpp"
#include "DashboardFrame.hpp"
#include "MailCreatorFrame.hpp"
#include "Session.hpp"
#include "MailFileManager.hpp"
#include "Exceptions.hpp"

class AppController : public wxEvtHandler {
protected:
    wxApp* _mainApp;
    Session* _session;

    // GUIs
    LoginFrame* _loginFrame;
    DashboardFrame* _dashboardFrame;
    bool _isMailCreatorOpen = false;
    MailCreatorFrame* _mailCreatorFrame;

    MailFileManager* _fileManager;
    
    std::vector<const Mail*> _currentMail;
    ssize_t _selectedMail = -1;

    // internal methods
    void login();
    void getSetMail(bool force = false);
    void warnUnimplemented();

    void showInfo(const std::string& msg);
    void showException(const std::string& msg);
public:
    AppController(wxApp* app, LoginFrame* loginFrame, DashboardFrame* dashboardFrame);
    ~AppController();
    
    void closeApp();
    void onCloseApp(wxCloseEvent& e);
    
    void onLoginSubmit(wxCommandEvent& e);

    void onSelectMail(wxCommandEvent& e);
    void onRefreshMailList(wxCommandEvent& e);
    void onNewMail(wxCommandEvent& e);
    void onReplyMail(wxCommandEvent& e);
    void onForwardMail(wxCommandEvent& e);
    void onDeleteMail(wxCommandEvent& e);
    void onAttachmentDownload(wxCommandEvent& e);

    void onMailCreatorSend(wxCommandEvent& e);
    void onMailCreatorClose(wxCommandEvent& e);
};