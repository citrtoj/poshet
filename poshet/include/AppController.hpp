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
#include "UsersManager.hpp"

class AppController : public wxEvtHandler, public SessionObserver, public MailBuilderObserver {
public:
    AppController(wxApp* app, LoginFrame* loginFrame, DashboardFrame* dashboardFrame, UsersFrame* usersFrame);
    ~AppController();
    
    void handleSessionDataUpdate() override;  
    void handleMailBuilderDataUpdate() override;

protected:
    wxApp* _mainApp;
    Session* _session = nullptr;
    UsersManager* _usersManager = nullptr;

    UsersFrame* _usersFrame = nullptr;
    LoginFrame* _loginFrame = nullptr;
    DashboardFrame* _dashboardFrame = nullptr;

    ssize_t _selectedUser = -1;

    void getSelectedUser() {
        _selectedUser = _usersFrame->selected();
    }

    const UserData& selectedUserData() {
        _selectedUser = _usersFrame->selected();
        return _usersManager->users()[_selectedUser];
    }
    void updateUserFrame();

    bool _isMailCreatorOpen = false;
    MailBuilder* _mailBuilder = nullptr;
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

    void onUsersAdd(wxCommandEvent& e);
    void onUsersEdit(wxCommandEvent& e);
    void onUsersDelete(wxCommandEvent& e);

    void onLoginAdd(wxCommandEvent& e);
    void onLoginEdit(wxCommandEvent& e);
    void onLogin(wxCommandEvent& e);
    void onLoginClose(wxCloseEvent& e);

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
};