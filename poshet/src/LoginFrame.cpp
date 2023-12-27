#include "LoginFrame.hpp"

wxTextCtrl* LoginFrame::addTextCtrlToSizer(wxWindow* parent, const std::string& labelText, wxBoxSizer* sizer, bool censored, bool allowOnlyNumbers) {
    auto label = (new wxStaticText(parent, wxID_ANY, labelText + ":"));
    sizer->Add(label, 0, wxEXPAND | wxALL, MARGIN);
    auto ctrl = new wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER
                                | (censored ? wxTE_PASSWORD : 0));
    sizer->Add(ctrl, 0, wxEXPAND | wxALL, MARGIN);
    return ctrl;
}
// wxCheckBox* LoginFrame::addCheckboxToSizer(wxWindow* parent, const std::string& labelText, wxBoxSizer* sizer) {
//     auto label = (new wxStaticText(parent, wxID_ANY, labelText));
//     // make a separate sizer in order to add them like. Horizontally if you will
// }

LoginFrame::LoginFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
    _statusBar = CreateStatusBar();
    SetStatusText("Ready");

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* inputsSizer = new wxBoxSizer(wxVERTICAL);
    inputsSizer->SetMinSize(wxSize(600, -1));

    _fullName = addTextCtrlToSizer(panel, "Full name (optional)", inputsSizer);
    _emailAddress = addTextCtrlToSizer(panel, "Email address", inputsSizer);
    _password = addTextCtrlToSizer(panel, "Password", inputsSizer, true);
    _pop3Domain = addTextCtrlToSizer(panel, "POP3 Domain (optional)", inputsSizer);
    _pop3Username = addTextCtrlToSizer(panel, "POP3 Username (optional)", inputsSizer);
    _smtpDomain = addTextCtrlToSizer(panel, "SMTP Domain (optional)", inputsSizer);
    
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

UserData LoginFrame::userInput() {
    
    return UserData(
        _fullName->GetValue().ToStdString(),
        _emailAddress->GetValue().ToStdString(),
        _password->GetValue().ToStdString(),
        _pop3Domain->GetValue().ToStdString(),
        _pop3Username->GetValue().ToStdString(),
        _smtpDomain->GetValue().ToStdString(),
        false,
        false,
        "",
        -1,
        -1
    );
}

wxButton* LoginFrame::loginButton() {
    return _loginButton;
}

void LoginFrame::showError(const std::string& errorString) {
    _statusBar->SetStatusText(errorString);
    wxMessageBox(errorString, "Error", wxOK | wxICON_ERROR);
}