#include "MailCreatorFrame.hpp"

wxDEFINE_EVENT(SEND_MAIL, wxCommandEvent);
wxDEFINE_EVENT(ADD_ATTACHMENT, wxCommandEvent);
wxDEFINE_EVENT(REMOVE_ATTACHMENT, wxCommandEvent);

MailCreatorFrame::MailCreatorFrame() :
    wxFrame(nullptr, wxID_ANY, "New mail", wxDefaultPosition, {800, 600})
{
    _mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    _mainPanel->SetBackgroundColour(wxColour(255, 255, 255, 255));
    auto verticalSizer = new wxBoxSizer(wxVERTICAL);

    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    _sendBtn = new wxButton(_mainPanel, wxID_ANY, "Send");
    _sendBtn->Bind(wxEVT_BUTTON, &MailCreatorFrame::OnSend, this);
    buttonsSizer->Add(_sendBtn, wxSizerFlags().Border(wxALL, MARGIN));
    _attachBtn = new wxButton(_mainPanel, wxID_ANY, "Attach");
    _attachBtn->Bind(wxEVT_BUTTON, &MailCreatorFrame::OnAttach, this);
    buttonsSizer->Add(_attachBtn, wxSizerFlags().Border(wxALL, MARGIN));

    verticalSizer->Add(buttonsSizer);

    auto headerSizer = new wxFlexGridSizer(2, 2, MARGIN, MARGIN);
    headerSizer->AddGrowableCol(1);

    headerSizer->Add(new wxStaticText(_mainPanel, wxID_ANY, "To"), 0, wxALIGN_CENTER_VERTICAL | wxALL, MARGIN);
    _toInput = new wxTextCtrl(_mainPanel, wxID_ANY);
    headerSizer->Add(_toInput, 0, wxEXPAND);

    headerSizer->Add(new wxStaticText(_mainPanel, wxID_ANY, "Subject"), 0, wxALIGN_CENTER_VERTICAL | wxALL, MARGIN);
    _subjectInput = new wxTextCtrl(_mainPanel, wxID_ANY);
    headerSizer->Add(_subjectInput, 0, wxEXPAND);

    verticalSizer->Add(headerSizer, 0, wxEXPAND | wxALL, MARGIN);

    _contentsCtrl = new wxRichTextCtrl(_mainPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);

    auto showHideAttBtn = new wxButton(_mainPanel, wxID_ANY, "Show/hide attachments");
    showHideAttBtn->Bind(wxEVT_BUTTON, &MailCreatorFrame::toggleAttachments, this);    

    _attContainer = new AttachmentsContainer(_mainPanel, wxBORDER_RAISED, "Remove", MARGIN);
    _attContainer->Hide();
    _attContainer->Bind(ATTACHMENT_CONTAINER_CLICK_EVENT, &MailCreatorFrame::OnRemove, this);    

    verticalSizer->Add(_contentsCtrl, 1, wxEXPAND | wxALL, MARGIN);
    verticalSizer->Add(showHideAttBtn, 0, wxALL, MARGIN);
    verticalSizer->Add(_attContainer, 0, wxEXPAND | wxALL, MARGIN);

    auto mainSizer = new wxBoxSizer(wxHORIZONTAL);
    // max-width: 75%;
    mainSizer->AddStretchSpacer(1);
    mainSizer->Add(verticalSizer, 6, wxEXPAND);
    mainSizer->AddStretchSpacer(1);

    _mainPanel->SetSizerAndFit(mainSizer);

    Centre();
}

bool MailCreatorFrame::closeGracefully() {
    return Close();
}

void MailCreatorFrame::OnSend(wxCommandEvent& event) {
    wxPostEvent(GetEventHandler(), wxCommandEvent(SEND_MAIL));
}

void MailCreatorFrame::OnAttach(wxCommandEvent& event) {
    wxPostEvent(GetEventHandler(), wxCommandEvent(ADD_ATTACHMENT));
}

void MailCreatorFrame::OnRemove(wxCommandEvent& event) {
    wxCommandEvent newEvent(REMOVE_ATTACHMENT);
    newEvent.SetInt(event.GetInt());
    wxPostEvent(GetEventHandler(), newEvent);
}

// --- getters ---

std::string MailCreatorFrame::to() const {
    return _toInput->GetValue().ToStdString();
}

std::string MailCreatorFrame::subject() const {
    return _subjectInput->GetValue().ToStdString();
}

std::string MailCreatorFrame::body() const {
    return _contentsCtrl->GetValue().ToStdString();
}

// --- setters ---

void MailCreatorFrame::setTo(const std::string& text) {
    _toInput->SetValue(wxString::FromUTF8(text.c_str(), text.length()));
}

void MailCreatorFrame::setSubject(const std::string& text) {
    _subjectInput->SetValue(wxString::FromUTF8(text.c_str(), text.length()));
}

void MailCreatorFrame::setBody(const std::string& text) {
    _contentsCtrl->SetValue(wxString::FromUTF8(text.c_str(), text.length()));
}

void MailCreatorFrame::setBodyCursorToBeginning() {
    _contentsCtrl->SetFocus();
    _contentsCtrl->SetInsertionPoint(-1);
}

void MailCreatorFrame::updateAttachments(const std::vector<AttachmentMetadata>& attachments) {
    _attContainer->clear();
    for (auto att : attachments) {
        _attContainer->addAttachment(att._filename, att._size);
    }
    _attContainer->Layout();
    _mainPanel->Layout();
}

void MailCreatorFrame::toggleAttachments(wxCommandEvent& e) {
    if (!_isAttContainerShowing) {
        _attContainer->Layout();
        _attContainer->Show();
        _attContainer->Layout();
        _attContainer->Refresh();
        _isAttContainerShowing = true;
    }
    else {
        _attContainer->Hide();
        _attContainer->Layout();
        _attContainer->Refresh();
        _isAttContainerShowing = false;
    }
    _mainPanel->Layout();
}