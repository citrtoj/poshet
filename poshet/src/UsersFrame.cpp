#include "UsersFrame.hpp"

wxDEFINE_EVENT(ADD_USER, wxCommandEvent);
wxDEFINE_EVENT(DELETE_USER, wxCommandEvent);
wxDEFINE_EVENT(EDIT_USER, wxCommandEvent);
wxDEFINE_EVENT(LOGIN_USER, wxCommandEvent);

UsersFrame::UsersFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title)
{
    _userListCtrl = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                   wxLC_REPORT | wxLC_SINGLE_SEL);

    _userListCtrl->InsertColumn(0, "Email Address", wxLIST_FORMAT_LEFT, 200);
    _userListCtrl->InsertColumn(1, "POP3 Server", wxLIST_FORMAT_LEFT, 200);

    _addButton = new wxButton(this, wxID_ANY, "Add user");
    _addButton->Bind(wxEVT_BUTTON, &UsersFrame::OnAddUser, this);
    _editButton = new wxButton(this, wxID_ANY, "Edit user info");
    _editButton->Bind(wxEVT_BUTTON, &UsersFrame::OnEditUserInfo, this);
    _deleteButton = new wxButton(this, wxID_ANY, "Delete user");
    _deleteButton->Bind(wxEVT_BUTTON, &UsersFrame::OnDeleteUser, this);
    _loginButton = new wxButton(this, wxID_ANY, "Login");
    _loginButton->Bind(wxEVT_BUTTON, &UsersFrame::OnLoginUser, this);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(_userListCtrl, 1, wxEXPAND | wxALL, MARGIN * 2);
    buttonsSizer->Add(_loginButton, 0,  wxALL, MARGIN);
    buttonsSizer->Add(_addButton, 0,  wxALL, MARGIN);
    buttonsSizer->Add(_editButton, 0,  wxALL, MARGIN);
    buttonsSizer->Add(_deleteButton, 0,  wxALL, MARGIN);
    mainSizer->Add(buttonsSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, MARGIN * 2);

    SetSizerAndFit(mainSizer);
    Centre();
    updateUserList();
}

long UsersFrame::selected() const {
    return _userListCtrl->GetFirstSelected();
}

void UsersFrame::setUsers(const std::vector<UserData>& data) {
    _users = data;
    updateUserList();
} 

void UsersFrame::updateUserList() {
    _userListCtrl->DeleteAllItems();

    for (size_t i = 0; i < _users.size(); ++i) {
        long index = _userListCtrl->InsertItem(i, _users[i].emailAddress());
        _userListCtrl->SetItem(index, 1, _users[i].pop3Domain());
    }
}

void UsersFrame::OnAddUser(wxCommandEvent& event) {
    wxCommandEvent newEvent(ADD_USER);
    wxPostEvent(GetEventHandler(), newEvent);
}

void UsersFrame::OnEditUserInfo(wxCommandEvent& event) {
    wxCommandEvent newEvent(EDIT_USER);
    wxPostEvent(GetEventHandler(), newEvent);
}

void UsersFrame::OnDeleteUser(wxCommandEvent& event) {
    wxCommandEvent newEvent(DELETE_USER);
    wxPostEvent(GetEventHandler(), newEvent);
}

void UsersFrame::OnLoginUser(wxCommandEvent& event) {
    wxCommandEvent newEvent(LOGIN_USER);
    wxPostEvent(GetEventHandler(), newEvent);
}

