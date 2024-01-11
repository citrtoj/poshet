#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>

#include "UserData.hpp"

wxDECLARE_EVENT(ADD_USER, wxCommandEvent);
wxDECLARE_EVENT(DELETE_USER, wxCommandEvent);
wxDECLARE_EVENT(EDIT_USER, wxCommandEvent);
wxDECLARE_EVENT(LOGIN_USER, wxCommandEvent);

class UsersFrame : public wxFrame {
public:
    UsersFrame(const wxString& title);

    long selected() const;
    void setUsers(const std::vector<UserData>& data);

protected:
    void updateUserList();
    static const int MARGIN = 5;

    void OnAddUser(wxCommandEvent& event);
    void OnEditUserInfo(wxCommandEvent& event);
    void OnDeleteUser(wxCommandEvent& event);
    void OnLoginUser(wxCommandEvent& event);

    std::vector<UserData> _users;

    wxListView* _userListCtrl;
    wxButton* _addButton;
    wxButton* _editButton;
    wxButton* _deleteButton;
    wxButton* _loginButton;
};