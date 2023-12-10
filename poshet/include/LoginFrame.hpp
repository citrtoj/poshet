#pragma once
#include <wx/wx.h>
#include <tuple>
#include <iostream>
#include <string>
#include "Session.hpp"
#include "LoginData.hpp"

class LoginFrame : public wxFrame {
protected:
    enum TextInputInfo {
        LABEL,
        STATICTEXT,
        TEXTCTRL,
        INPUTTYPE,
        CENSOR
    };

    enum InputType {
        SINGLE,
        MULTI
    };

    enum Censor {
        UNCENSORED,
        CENSORED
    };
    std::vector<std::tuple<std::string, wxStaticText*, wxTextCtrl*, InputType, Censor>> _textInputs;
    std::vector<wxStaticText*> _textLabels;
    std::vector<wxTextCtrl*> _textControls;
    wxButton* _loginButton;
    wxStatusBar* _statusBar;

    void OnLogin(wxCommandEvent& event);

public:
    LoginFrame(const wxString& title);
    std::vector<std::string> userInput();
    wxButton* loginButton();
    
    void showError(const std::string& errorString);
};
