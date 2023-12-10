#include "LoginFrame.hpp"

LoginFrame::LoginFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize),
    _textInputs({
        {"Full name", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::UNCENSORED},
        {"Username", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::UNCENSORED},
        {"Password", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::CENSORED},
        {"POP3 Domain Name", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::UNCENSORED},
        {"SMTP Domain Name", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::UNCENSORED},
    })
{
    _statusBar = CreateStatusBar();
    SetStatusText("Ready");
    const auto MARGIN = FromDIP(5);

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->AddStretchSpacer();
    for (auto& textInputInfo : _textInputs) {
        auto label = (new wxStaticText(panel, wxID_ANY, std::get<LoginFrame::LABEL>(textInputInfo) + ":"));
        sizer->Add(label, 0, wxEXPAND | wxALL, MARGIN);
        std::get<LoginFrame::STATICTEXT>(textInputInfo) = label;

        auto ctrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER
                                    | (std::get<LoginFrame::INPUTTYPE>(textInputInfo) == LoginFrame::SINGLE ? 0 : wxTE_MULTILINE)
                                    | (std::get<LoginFrame::CENSOR>(textInputInfo) == LoginFrame::UNCENSORED ? 0 : wxTE_PASSWORD));
        sizer->Add(ctrl, 0, wxEXPAND | wxALL, MARGIN);
        std::get<LoginFrame::TEXTCTRL>(textInputInfo) = ctrl;
    }
    
    _loginButton = new wxButton(panel, wxID_ANY, "Login");
    sizer->Add(_loginButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, MARGIN);
    sizer->AddStretchSpacer();

    sizer->SetMinSize(wxSize(800, -1));

    panel->SetSizerAndFit(sizer);

    wxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(panel, 1, wxALIGN_CENTER | wxALL, 100);
    SetSizerAndFit(frameSizer);
    Centre();
    
    //_loginButton->Bind(wxEVT_BUTTON, &LoginFrame::OnLogin, this);
}

std::vector<std::string> LoginFrame::userInput() {
    std::vector<std::string> loginDataDump;
    for (auto& input : _textInputs) {
        loginDataDump.push_back(std::get<LoginFrame::TEXTCTRL>(input)->GetValue().ToStdString());
    }
    return loginDataDump;
}

wxButton* LoginFrame::loginButton() {
    return _loginButton;
}

void LoginFrame::showError(const std::string& errorString) {
    _statusBar->SetStatusText(errorString);
    wxMessageBox(errorString, "Error", wxOK | wxICON_ERROR);
}