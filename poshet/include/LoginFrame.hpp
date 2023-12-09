#pragma once
#include <wx/wx.h>
#include <tuple>
#include <iostream>
#include <string>
#include "Session.hpp"
#include "LoginData.hpp"

namespace LoginView {
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
}


class LoginFrame : public wxFrame {
private:
    std::vector<std::tuple<std::string, wxStaticText*, wxTextCtrl*, LoginView::InputType, LoginView::Censor>> _textInputs;
    std::vector<wxStaticText*> _textLabels;
    std::vector<wxTextCtrl*> _textControls;
    wxButton* _loginButton;
    wxStatusBar* _statusBar;

    void OnLogin(wxCommandEvent& event);
public:
    LoginFrame(const wxString& title);
    std::vector<std::string> userInput();
    wxButton* loginButton();
};
