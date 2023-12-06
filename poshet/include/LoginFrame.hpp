#pragma once

#include <wx/wx.h>

class LoginFrame : public wxFrame {
public:
    LoginFrame(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200)) {

        wxPanel* panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        wxStaticText* usernameLabel = new wxStaticText(panel, wxID_ANY, "Username:");
        _usernameTextCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        wxStaticText* passwordLabel = new wxStaticText(panel, wxID_ANY, "Password:");
        _passwordTextCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD | wxTE_PROCESS_ENTER);
        _loginButton = new wxButton(panel, wxID_ANY, "Login");

        sizer->Add(usernameLabel, 0, wxALL | wxEXPAND, 5);
        sizer->Add(_usernameTextCtrl, 0, wxALL | wxEXPAND, 5);
        sizer->Add(passwordLabel, 0, wxALL | wxEXPAND, 5);
        sizer->Add(_passwordTextCtrl, 0, wxALL | wxEXPAND, 5);
        sizer->Add(_loginButton, 0, wxALL | wxALIGN_CENTER, 5);

        panel->SetSizerAndFit(sizer);
        Centre();

        _loginButton->Bind(wxEVT_BUTTON, &LoginFrame::OnLogin, this);
        _usernameTextCtrl->Bind(wxEVT_TEXT_ENTER, &LoginFrame::OnLogin, this);
        _passwordTextCtrl->Bind(wxEVT_TEXT_ENTER, &LoginFrame::OnLogin, this);
    }

private:
    wxTextCtrl* _usernameTextCtrl;
    wxTextCtrl* _passwordTextCtrl;
    wxButton* _loginButton;

    void OnLogin(wxCommandEvent& event) {
        wxString username = _usernameTextCtrl->GetValue();
        wxString password = _passwordTextCtrl->GetValue();
        // placeholder
        if (username == "user" && password == "password") {
            wxMessageBox("Login Successful!", "Success", wxOK | wxICON_INFORMATION);
        } else {
            wxMessageBox("Login Failed. Please check your credentials.", "Error", wxOK | wxICON_ERROR);
        }
    }
};
