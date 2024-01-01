#include "MailCreatorFrame.hpp"

wxDEFINE_EVENT(SEND_MAIL, wxCommandEvent);

MailCreatorFrame::MailCreatorFrame() :
    wxFrame(nullptr, wxID_ANY, "New mail")
{
    auto mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    mainPanel->SetBackgroundColour(wxColour(255, 255, 255, 255));
    auto verticalSizer = new wxBoxSizer(wxVERTICAL);

    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    _sendBtn = new wxButton(mainPanel, wxID_ANY, "Send");
    _sendBtn->Bind(wxEVT_BUTTON, &MailCreatorFrame::OnSend, this);
    buttonsSizer->Add(_sendBtn, wxSizerFlags().Border(wxALL, MARGIN));
    _attachBtn = new wxButton(mainPanel, wxID_ANY, "Attach");
    buttonsSizer->Add(_attachBtn, wxSizerFlags().Border(wxALL, MARGIN));

    verticalSizer->Add(buttonsSizer);

    auto headerSizer = new wxFlexGridSizer(2, 2, MARGIN, MARGIN);
    headerSizer->AddGrowableCol(1);

    headerSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "To"), 0, wxALIGN_CENTER_VERTICAL | wxALL, MARGIN);
    _toInput = new wxTextCtrl(mainPanel, wxID_ANY);
    headerSizer->Add(_toInput, 0, wxEXPAND);

    headerSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Subject"), 0, wxALIGN_CENTER_VERTICAL | wxALL, MARGIN);
    _subjectInput = new wxTextCtrl(mainPanel, wxID_ANY);
    headerSizer->Add(_subjectInput, 0, wxEXPAND);

    verticalSizer->Add(headerSizer, 0, wxEXPAND | wxALL, MARGIN);

    _contentsCtrl = new wxRichTextCtrl(mainPanel, wxID_ANY, "", wxDefaultPosition, wxSize(400, 600), wxBORDER_SUNKEN);

    verticalSizer->Add(_contentsCtrl, wxSizerFlags().Border(wxALL, MARGIN).Expand());

    auto mainSizer = new wxBoxSizer(wxHORIZONTAL);
    // max-width: 75%;
    mainSizer->AddStretchSpacer(1);
    mainSizer->Add(verticalSizer, 6);
    mainSizer->AddStretchSpacer(1);

    mainPanel->SetSizerAndFit(mainSizer);

    Centre();
}

bool MailCreatorFrame::closeGracefully() {
    return Close();
}


void MailCreatorFrame::OnSend(wxCommandEvent& event) {
    wxPostEvent(GetEventHandler(), wxCommandEvent(SEND_MAIL));
}

// void MailCreatorFrame::OnClose(wxEvent& event) {
//     // wxStringOutputStream strm;
//     // wxRichTextBuffer buffer(_contentsCtrl->GetBuffer());
//     // wxRichTextXMLHandler xmlHandler;
//     // // mă simt de parcă trag cu dinții de output-ul ăsta
//     // xmlHandler.ExportXML(*(wxOutputStream*)(&strm), *(wxRichTextObject*)(_contentsCtrl->GetFocusObject()), 0);
//     // std::cout << strm.GetString();
//     //_subscriber->onMailCreatorClose();  //notifies in case it needs to save input, or something...
//     event.Skip();
// }

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