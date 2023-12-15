#include "LoginFrame.hpp"

LoginFrame::LoginFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize),
    _textInputs({
        {"Full name (optional)", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::UNCENSORED},
        {"Email address", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::UNCENSORED},
        {"Password", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::CENSORED},
        {"POP3 Domain Name (optional)", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::UNCENSORED},
        {"POP3 Username (optional)", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::UNCENSORED},
        {"SMTP Domain Name (optional)", nullptr, nullptr, LoginFrame::InputType::SINGLE, LoginFrame::UNCENSORED},
    })
{
    _statusBar = CreateStatusBar();
    SetStatusText("Ready");

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* inputsSizer = new wxBoxSizer(wxVERTICAL);
    inputsSizer->SetMinSize(wxSize(600, -1));

    for (auto& textInputInfo : _textInputs) {
        auto label = (new wxStaticText(panel, wxID_ANY, std::get<LoginFrame::LABEL>(textInputInfo) + ":"));
        inputsSizer->Add(label, 0, wxEXPAND | wxALL, MARGIN);
        std::get<LoginFrame::STATICTEXT>(textInputInfo) = label;

        auto ctrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER
                                    | (std::get<LoginFrame::INPUTTYPE>(textInputInfo) == LoginFrame::SINGLE ? 0 : wxTE_MULTILINE)
                                    | (std::get<LoginFrame::CENSOR>(textInputInfo) == LoginFrame::UNCENSORED ? 0 : wxTE_PASSWORD));
        inputsSizer->Add(ctrl, 0, wxEXPAND | wxALL, MARGIN);
        std::get<LoginFrame::TEXTCTRL>(textInputInfo) = ctrl;
    }
    
    _loginButton = new wxButton(panel, wxID_ANY, "Login");
    inputsSizer->Add(_loginButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, MARGIN);

    auto horizontalSizer = new wxBoxSizer(wxHORIZONTAL);

    horizontalSizer->AddStretchSpacer(1);
    horizontalSizer->Add(inputsSizer, 10, wxALIGN_CENTER_VERTICAL);
    horizontalSizer->AddStretchSpacer(1);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    
    mainSizer->AddStretchSpacer(1);
    mainSizer->Add(horizontalSizer, 10);
    mainSizer->AddStretchSpacer(1);

    panel->SetSizerAndFit(mainSizer);

    wxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(panel, 1, wxALIGN_CENTER | wxALL, 100);
    SetSizerAndFit(frameSizer);
    Centre();
    
    _loginButton->Bind(wxEVT_BUTTON, &LoginFrame::OnLogin, this);
    this->Bind(wxEVT_TEXT_ENTER, &LoginFrame::OnLogin, this);
    this->Bind(wxEVT_CLOSE_WINDOW, &LoginFrame::OnClose, this);
}

void LoginFrame::OnLogin(wxCommandEvent& event) {
    _subscriber->onLoginSubmit();
}

void LoginFrame::OnClose(wxEvent& event) {
    _subscriber->onCloseApp();
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