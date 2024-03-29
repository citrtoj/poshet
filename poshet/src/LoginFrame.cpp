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

    _loginButton = new wxButton(panel, wxID_ANY, "Save user info");
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

void LoginFrame::setInput(const UserData& data) {
    _fullName->SetValue(wxString::FromUTF8(data.fullName().c_str()));
    _emailAddress->SetValue(wxString::FromUTF8(data.emailAddress().c_str()));
    _password->SetValue(wxString::FromUTF8(data.password().c_str()));
    _pop3Domain->SetValue(wxString::FromUTF8(data.pop3Domain().c_str()));
    _pop3Port->SetValue(wxString::FromUTF8(data.pop3Port().c_str()));
    _pop3Username->SetValue(wxString::FromUTF8(data.pop3Username().c_str()));
    _smtpDomain->SetValue(wxString::FromUTF8(data.smtpDomain().c_str()));
    _smtpPort->SetValue(wxString::FromUTF8(data.smtpPort().c_str()));
    _smtpUsername->SetValue(wxString::FromUTF8(data.smtpUsername().c_str()));
    _pop3SSL->SetValue(data.pop3SSL());
    _smtpSSL->SetValue(data.smtpSSL());
    _smtpAuth->SetValue(data.smtpAuth());
}

void LoginFrame::clear() {
    _fullName->SetValue("");
    _emailAddress->SetValue("");
    _password->SetValue("");
    _pop3Domain->SetValue("");
    _pop3Port->SetValue("");
    _pop3Username->SetValue("");
    _smtpDomain->SetValue("");
    _smtpPort->SetValue("");
    _smtpUsername->SetValue("");
    _pop3SSL->SetValue(false);
    _smtpSSL->SetValue(false);
    _smtpAuth->SetValue(false);
}

void LoginFrame::OnLogin(wxCommandEvent& event) {
    wxCommandEvent newEvent(LOGIN_SUBMIT);
    wxPostEvent(GetEventHandler(), newEvent);
}

UserData LoginFrame::userInput() {
    return UserData(
        _emailAddress->GetValue().ToUTF8().data(),
        _fullName->GetValue().ToUTF8().data(),
        _password->GetValue().ToUTF8().data(),

        _pop3SSL->GetValue(),
        _pop3Domain->GetValue().ToUTF8().data(),
        _pop3Port->GetValue().ToUTF8().data(),
        _pop3Username->GetValue().ToUTF8().data(),

        _smtpSSL->GetValue(),
        _smtpDomain->GetValue().ToUTF8().data(),
        _smtpPort->GetValue().ToUTF8().data(),
        _smtpAuth->GetValue(),
        _smtpUsername->GetValue().ToUTF8().data()
    );
}

wxButton* LoginFrame::loginButton() {
    return _loginButton;
}

void LoginFrame::setTitle(const std::string& msg) {
    SetTitle(msg);
    loginButton()->SetLabel(msg);
}

void LoginFrame::showError(const std::string& errorString) {
    SetStatusText(errorString);
    wxMessageBox(errorString, "Error", wxOK | wxICON_ERROR);
}

void LoginFrame::showMessage(const std::string& msg) {
    SetStatusText(msg);
}