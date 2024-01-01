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

class AppController : public wxEvtHandler, public SessionObserver, public MailBodyBuilderObserver {

protected:
    wxApp* _mainApp;
    Session* _session;

    // GUIs
    LoginFrame* _loginFrame;
    DashboardFrame* _dashboardFrame;

    bool _isMailCreatorOpen = false;
    MailBodyBuilder* _mailBuilder;
    MailCreatorFrame* _mailCreatorFrame;
    bool createMailCreatorFrame();

    MailFileManager* _fileManager;
    
    std::vector<const Mail*> _currentMail;
    ssize_t _selectedMail = -1;

    // display an error, wxWidgets style
    void showInfo(const std::string& msg);
    void showException(const std::string& msg);

    // placeholder for things i haven't added yet
    void warnUnimplemented();

    // wxWidgets-style app closing
    void closeApp();

    // internal login method
    void login();

    void getMailAndShow(bool force = false);
    
    void onCloseApp(wxCloseEvent& e);
    void onLoginSubmit(wxCommandEvent& e);
    void onSelectMail(wxCommandEvent& e);
    void onRefreshMailList(wxCommandEvent& e);
    void onViewMailWithTag(wxCommandEvent& e);
    void onViewAllMail(wxCommandEvent& e);
    void onNewMail(wxCommandEvent& e);
    void onTagMail(wxCommandEvent& e);
    void onReplyMail(wxCommandEvent& e);
    void onForwardMail(wxCommandEvent& e);
    void onDeleteMail(wxCommandEvent& e);
    void onAttachmentDownload(wxCommandEvent& e);

    void onMailCreatorSend(wxCommandEvent& e);
    void onMailCreatorClose(wxCloseEvent& e);

public:
    AppController(wxApp* app, LoginFrame* loginFrame, DashboardFrame* dashboardFrame);
    ~AppController();

public:
    // model observer methods
    void handleSessionDataUpdate() override;  

    // the only time that the underlying data would change would be when attaching attachments, so it's fine that it's a single fxn
    // the reply/forward-specific data is added to the user input when creating the actual frame
    void handleMailBuilderDataUpdate() override;
};