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
    //static wxCheckBox* addCheckboxToSizer(wxWindow* parent, const std::string& labelText, wxBoxSizer* sizer);

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

    //LoginFrameSubscriber* _subscriber;

    // std::vector<std::tuple<std::string, wxStaticText*, wxTextCtrl*, Censor>> _textInputs;
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

    void OnLogin(wxCommandEvent& event);

public:
    LoginFrame(const wxString& title);
    UserData userInput();
    wxButton* loginButton();

    // void subscribe(LoginFrameSubscriber* sub) {
    //     _subscriber = sub;
    // }

    void showError(const std::string& errorString);
};
