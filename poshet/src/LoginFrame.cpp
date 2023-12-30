#include "LoginFrame.hpp"

wxDEFINE_EVENT(LOGIN_SUBMIT, wxCommandEvent);

wxTextCtrl* LoginFrame::addTextCtrlToSizer(wxWindow* parent, const std::string& labelText, wxBoxSizer* sizer,  long styleFlags, bool numbersOnly) {
    auto label = (new wxStaticText(parent, wxID_ANY, labelText + ":"));
    sizer->Add(label, 0, wxEXPAND | wxALL, MARGIN);
    
    wxTextValidator validator(wxFILTER_DIGITS);
    
    auto ctrl = new wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | styleFlags, numbersOnly ? validator : wxDefaultValidator);
    sizer->Add(ctrl, 0, wxEXPAND | wxALL, MARGIN);
    return ctrl;
}

LoginFrame::LoginFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
    _statusBar = CreateStatusBar();
    SetStatusText("Ready");

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* inputsSizer = new wxBoxSizer(wxVERTICAL);

    _emailAddress = addTextCtrlToSizer(panel, "Email address", inputsSizer);
    _fullName = addTextCtrlToSizer(panel, "Full name (default: local part of email address)", inputsSizer);
    _password = addTextCtrlToSizer(panel, "Password", inputsSizer, wxTE_PASSWORD);

    auto twoColumnHorizSizer = new wxBoxSizer(wxHORIZONTAL);
    
    auto pop3Sizer = new wxBoxSizer(wxVERTICAL);
    _pop3Port = addTextCtrlToSizer(panel, "POP3 Port (default: 110 / 995 for SSL)", pop3Sizer, 0, true);
    _pop3SSL = new wxCheckBox(panel, wxID_ANY, "Enable SSL for POP3");
    pop3Sizer->Add(_pop3SSL, 0, wxALL, MARGIN);
    _pop3Domain = addTextCtrlToSizer(panel, "POP3 Domain (default: domain of email address)", pop3Sizer);
    _pop3Username = addTextCtrlToSizer(panel, "POP3 Username (default: email address)", pop3Sizer);

    auto smtpSizer = new wxBoxSizer(wxVERTICAL);
    _smtpPort = addTextCtrlToSizer(panel, "SMTP Port (default: 25 / 465 for SSL)", smtpSizer, 0, true);
    _smtpSSL = new wxCheckBox(panel, wxID_ANY, "Enable SSL for SMTP");
    smtpSizer->Add(_smtpSSL, 0, wxALL, MARGIN);
    _smtpDomain = addTextCtrlToSizer(panel, "SMTP Domain (default: domain of email address)", smtpSizer);
    _smtpAuth = new wxCheckBox(panel, wxID_ANY, "Enable SMTP authentication (AUTH LOGIN)");
    smtpSizer->Add(_smtpAuth, 0, wxALL, MARGIN);
    _smtpUsername = addTextCtrlToSizer(panel, "SMTP Username (default: email address)", smtpSizer);

    twoColumnHorizSizer->Add(pop3Sizer, 1, wxEXPAND);
    twoColumnHorizSizer->Add(smtpSizer, 1, wxEXPAND);

    inputsSizer->Add(twoColumnHorizSizer, 0, wxEXPAND);

    _loginButton = new wxButton(panel, wxID_ANY, "Login");
    inputsSizer->Add(_loginButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, MARGIN);

    auto horizontalSizer = new wxBoxSizer(wxHORIZONTAL);

    horizontalSizer->AddStretchSpacer(1);
    horizontalSizer->Add(inputsSizer, 0, wxALIGN_CENTER_VERTICAL);
    horizontalSizer->AddStretchSpacer(1);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    
    mainSizer->Add(horizontalSizer, 1);

    panel->SetSizerAndFit(mainSizer);

    wxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(panel, 1, wxALIGN_CENTER | wxALL, 100);
    SetSizerAndFit(frameSizer);
    Centre();
    
    _loginButton->Bind(wxEVT_BUTTON, &LoginFrame::OnLogin, this);
    this->Bind(wxEVT_TEXT_ENTER, &LoginFrame::OnLogin, this);
}

void LoginFrame::OnLogin(wxCommandEvent& event) {
    wxCommandEvent newEvent(LOGIN_SUBMIT);
    wxPostEvent(GetEventHandler(), newEvent);
    //_subscriber->onLoginSubmit();
}

UserData LoginFrame::userInput() {
    return UserData(
        _emailAddress->GetValue().ToStdString(),
        _fullName->GetValue().ToStdString(),
        _password->GetValue().ToStdString(),

        _pop3SSL->GetValue(),
        _pop3Domain->GetValue().ToStdString(),
        _pop3Port->GetValue().ToStdString(),
        _pop3Username->GetValue().ToStdString(),

        _smtpSSL->GetValue(),
        _smtpDomain->GetValue().ToStdString(),
        _smtpPort->GetValue().ToStdString(),
        _smtpAuth->GetValue(),
        _smtpUsername->GetValue().ToStdString()
    );
}

wxButton* LoginFrame::loginButton() {
    return _loginButton;
}

void LoginFrame::showError(const std::string& errorString) {
    _statusBar->SetStatusText(errorString);
    wxMessageBox(errorString, "Error", wxOK | wxICON_ERROR);
}