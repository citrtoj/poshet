#pragma once

#include <string>
#include <iostream>

#include <wx/wx.h>

#include "Mail.hpp"
#include "LoginFrame.hpp"
#include "DashboardFrame.hpp"
#include "MailCreatorFrame.hpp"
#include "UsersFrame.hpp"
#include "Session.hpp"
#include "FileManager.hpp"
#include "Exceptions.hpp"

class AppController : public wxEvtHandler, public SessionObserver, public MailBodyBuilderObserver {
protected:
    wxApp* _mainApp;
    Session* _session = nullptr;

    UsersFrame* _usersFrame = nullptr;
    LoginFrame* _loginFrame = nullptr;
    DashboardFrame* _dashboardFrame = nullptr;

    std::vector<UserData> _users;
    ssize_t _selectedUser = -1;
    UserData& selectedUserData() {
        return _users[_selectedUser];
    }
    const UserData& selectedUserData() const {
        return _users[_selectedUser];
    }
    void getUsers();

    bool _isMailCreatorOpen = false;
    MailBodyBuilder* _mailBuilder = nullptr;
    MailCreatorFrame* _mailCreatorFrame = nullptr;
    bool createMailCreatorFrame();
    void initNewMailCreatorFrame();
    void initReplyMailCreatorFrame();
    void initForwardMailCreatorFrame();
    void onNewMail(wxCommandEvent& e);
    void onReplyMail(wxCommandEvent& e);
    void onForwardMail(wxCommandEvent& e);

    void closeMailCreator();

    FileManager* _fileManager = nullptr;
    
    std::vector<const Mail*> _currentMail = {};
    ssize_t _selectedMail = -1;

    void showInfo(const std::string& msg);
    void showException(const std::string& msg);

    void closeApp();

    void login();
    void getMailAndShow(bool force = false);
    
    void onCloseApp(wxCloseEvent& e);
    void onLoginSubmit(wxCommandEvent& e);
    void onSelectMail(wxCommandEvent& e);
    void onRefreshMailList(wxCommandEvent& e);
    void onViewMailWithTag(wxCommandEvent& e);
    void onViewAllMail(wxCommandEvent& e);
    void onTagMail(wxCommandEvent& e);
    void onDeleteMail(wxCommandEvent& e);
    void onAttachmentDownload(wxCommandEvent& e);

    void onMailCreatorSend(wxCommandEvent& e);
    void onMailCreatorClose(wxCloseEvent& e);
    void onMailCreatorAddAttachment(wxCommandEvent& e);
    void onMailCreatorRemoveAttachment(wxCommandEvent& e);

    void onUsersAdd(wxCommandEvent& e);
    void onUsersEdit(wxCommandEvent& e);

    // void onLoginAdd(wxCommandEvent& e);
    // void onLoginEdit(wxCommandEvent& e);


public:
    AppController(wxApp* app, LoginFrame* loginFrame, DashboardFrame* dashboardFrame, UsersFrame* usersFrame);
    ~AppController();
    
    void handleSessionDataUpdate() override;  
    void handleMailBuilderDataUpdate() override;
};