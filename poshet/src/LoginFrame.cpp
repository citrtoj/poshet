#include "LoginFrame.hpp"

LoginFrame::LoginFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize),
    _textInputs({
        {"Full name", nullptr, nullptr, LoginView::InputType::SINGLE, LoginView::UNCENSORED},
        {"Username", nullptr, nullptr, LoginView::InputType::SINGLE, LoginView::UNCENSORED},
        {"Password", nullptr, nullptr, LoginView::InputType::SINGLE, LoginView::CENSORED},
        {"POP3 Domain Name", nullptr, nullptr, LoginView::InputType::SINGLE, LoginView::UNCENSORED},
        {"SMTP Domain Name", nullptr, nullptr, LoginView::InputType::SINGLE, LoginView::UNCENSORED},
    })
{
    _statusBar = CreateStatusBar();
    SetStatusText("Ready");
    const auto MARGIN = FromDIP(5);

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->AddStretchSpacer();
    for (auto& textInputInfo : _textInputs) {
        auto label = (new wxStaticText(panel, wxID_ANY, std::get<LoginView::LABEL>(textInputInfo) + ":"));
        sizer->Add(label, 0, wxEXPAND | wxALL, MARGIN);
        std::get<LoginView::STATICTEXT>(textInputInfo) = label;

        auto ctrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER
                                    | (std::get<LoginView::INPUTTYPE>(textInputInfo) == LoginView::SINGLE ? 0 : wxTE_MULTILINE)
                                    | (std::get<LoginView::CENSOR>(textInputInfo) == LoginView::UNCENSORED ? 0 : wxTE_PASSWORD));
        sizer->Add(ctrl, 0, wxEXPAND | wxALL, MARGIN);
        std::get<LoginView::TEXTCTRL>(textInputInfo) = ctrl;
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
        loginDataDump.push_back(std::get<LoginView::TEXTCTRL>(input)->GetValue().ToStdString());
    }
    return loginDataDump;
}

wxButton* LoginFrame::loginButton() {
    return _loginButton;
}
