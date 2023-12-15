#include "MailCreatorFrame.hpp"

MailCreatorFrame::MailCreatorFrame() :
    wxFrame(nullptr, wxID_ANY, "New mail")
{
    this->Bind(wxEVT_CLOSE_WINDOW, &MailCreatorFrame::OnClose, this);

    auto mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    mainPanel->SetBackgroundColour(wxColour(255, 255, 255, 255));
    auto verticalSizer = new wxBoxSizer(wxVERTICAL);

    // BUTTONS SIZER: _sendBtn, _attachBtn

    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    _sendBtn = new wxButton(mainPanel, wxID_ANY, "Send");
    _sendBtn->Bind(wxEVT_BUTTON, &MailCreatorFrame::OnSend, this);
    buttonsSizer->Add(_sendBtn, wxSizerFlags().Border(wxALL, MARGIN));
    _attachBtn = new wxButton(mainPanel, wxID_ANY, "Attach");
    buttonsSizer->Add(_attachBtn, wxSizerFlags().Border(wxALL, MARGIN));

    verticalSizer->Add(buttonsSizer);

    //HEADER SIZER: To, Subject

    auto headerSizer = new wxFlexGridSizer(2, 2, MARGIN, MARGIN);
    headerSizer->AddGrowableCol(1);

    headerSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "To"), 0, wxALIGN_CENTER_VERTICAL | wxALL, MARGIN);
    _toInput = new wxTextCtrl(mainPanel, wxID_ANY);
    headerSizer->Add(_toInput, 0, wxEXPAND);

    headerSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Subject"), 0, wxALIGN_CENTER_VERTICAL | wxALL, MARGIN);
    _subjectInput = new wxTextCtrl(mainPanel, wxID_ANY);
    headerSizer->Add(_subjectInput, 0, wxEXPAND);

    verticalSizer->Add(headerSizer, 0, wxEXPAND | wxALL, MARGIN);

    // ADD RICHTEXTCTRL

    _contentsCtrl = new wxRichTextCtrl(mainPanel, wxID_ANY, "", wxDefaultPosition, wxSize(400, 600), wxBORDER_SUNKEN);
    verticalSizer->Add(_contentsCtrl, wxSizerFlags().Border(wxALL, MARGIN).Expand());

    auto mainSizer = new wxBoxSizer(wxHORIZONTAL);
    // max-width: 75%%;
    mainSizer->AddStretchSpacer(1);
    mainSizer->Add(verticalSizer, 6);
    mainSizer->AddStretchSpacer(1);

    mainPanel->SetSizerAndFit(mainSizer);

    Centre();
}

void MailCreatorFrame::subscribe(MailCreatorFrameSubscriber* sub) {
    _subscriber = sub;
}

bool MailCreatorFrame::closeGracefully() {
    return Close();
}

// wxWidgets handlers

void MailCreatorFrame::OnSend(wxCommandEvent& event) {
    _subscriber->onMailCreatorSend();
}

void MailCreatorFrame::OnClose(wxEvent& event) {
    _subscriber->onMailCreatorClose();  //notifies in case it needs to save input, or something...
    event.Skip();
}

// getters. temporary

std::string MailCreatorFrame::to() const {
    return _toInput->GetValue().ToStdString();
}

std::string MailCreatorFrame::subject() const {
    return _subjectInput->GetValue().ToStdString();
}

std::string MailCreatorFrame::body() const {
    return _contentsCtrl->GetValue().ToStdString();
}