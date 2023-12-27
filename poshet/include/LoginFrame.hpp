#pragma once

#include <tuple>
#include <iostream>
#include <string>

#include <wx/wx.h>

#include "Session.hpp"
#include "UserData.hpp"
#include "FrameSubscribers.hpp"

class LoginFrame : public wxFrame {
protected:
    static wxTextCtrl* addTextCtrlToSizer(wxWindow* parent, const std::string& labelText, wxBoxSizer* sizer, bool censored = false, bool allowOnlyNumbers = false);
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

    LoginFrameSubscriber* _subscriber;

    // std::vector<std::tuple<std::string, wxStaticText*, wxTextCtrl*, Censor>> _textInputs;
    wxTextCtrl* _fullName, *_emailAddress, *_password, *_pop3Domain, *_pop3Username, *_smtpDomain;
    wxButton* _loginButton;
    wxStatusBar* _statusBar;

    static const int MARGIN = 5;

    void OnLogin(wxCommandEvent& event);
    void OnClose(wxEvent& event);

public:
    LoginFrame(const wxString& title);
    UserData userInput();
    wxButton* loginButton();

    void subscribe(LoginFrameSubscriber* sub) {
        _subscriber = sub;
    }

    void showError(const std::string& errorString);
};
