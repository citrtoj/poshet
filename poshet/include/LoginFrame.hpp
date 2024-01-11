#pragma once

#include <tuple>
#include <iostream>
#include <string>

#include <wx/wx.h>

#include "Session.hpp"
#include "UserData.hpp"

wxDECLARE_EVENT(LOGIN_SUBMIT, wxCommandEvent);

class LoginFrame : public wxFrame {
protected:
    static wxTextCtrl* addTextCtrlToSizer(wxWindow* parent, const std::string& labelText, wxBoxSizer* sizer, long styleFlags = 0, bool numbersOnly = false);

    enum TextInputInfo {
        LABEL,
        STATICTEXT,
        TEXTCTRL,
        INPUTTYPE,
        CENSOR
    };

    enum Censor {
        UNCENSORED,
        CENSORED
    };

    wxTextCtrl *_fullName, *_emailAddress, *_password;
    wxCheckBox *_pop3SSL;
    wxTextCtrl *_pop3Domain, *_pop3Port, *_pop3Username;
    wxCheckBox *_smtpSSL;
    wxTextCtrl *_smtpDomain, *_smtpPort;
    wxCheckBox *_smtpAuth;
    wxTextCtrl *_smtpUsername;
    
    wxButton* _loginButton;
    wxStatusBar* _statusBar;
    static const int MARGIN = 5;
    wxButton* loginButton();
    void OnLogin(wxCommandEvent& event);

public:
    LoginFrame(const wxString& title);
    UserData userInput();

    void setTitle(const std::string& msg);

    void setInput(const UserData& data);
    void clear();
    
    void showError(const std::string& errorString);
    void showMessage(const std::string& msg);
};
