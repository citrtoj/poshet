#include "GUIComponents.hpp"

wxDEFINE_EVENT(ATTACHMENT_BUTTON_CLICK_EVENT, wxCommandEvent);

AttachmentPanel::AttachmentPanel(wxWindow* parent, int index, const wxString& attachmentName, unsigned long long fileSize, int MARGIN) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED) {
    _sizer = new wxBoxSizer(wxHORIZONTAL);
    _text = new wxStaticText(this, wxID_ANY, attachmentLabel(attachmentName, fileSize));
    _sizer->Add(_text, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2 * MARGIN);
    // bind mousedown on panel to attachment click
    // Bind(wxEVT_LEFT_DOWN, &AttachmentPanel::onAttachmentPanelClick, this);

    this->SetSizerAndFit(_sizer);
    _attachmentName = attachmentName;
    _index = index;
    _fileSize = fileSize;
}

// void AttachmentPanel::onAttachmentPanelClick(wxMouseEvent& event) {
//     wxCommandEvent newEvent(ATTACHMENT_CLICK_EVENT);
//     newEvent.SetInt(_index);
//     wxPostEvent(GetEventHandler(), newEvent);
// }

wxString AttachmentPanel::attachmentLabel(const wxString& attachment, unsigned long long fileSize) {
    return wxString::Format("%-20s %20s", attachment, Utils::fileSizeToString(fileSize));
}

AttachmentPanelWithButton::AttachmentPanelWithButton(wxWindow* parent, int index, const wxString& attachmentName, unsigned long long fileSize, const wxString& buttonLabel, int MARGIN) : AttachmentPanel(parent, index, attachmentName, fileSize, MARGIN) {
    _downloadButton = new wxButton(this, wxID_ANY, buttonLabel, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxButtonNameStr);

    _sizer->Add(_downloadButton, 0, wxALL, MARGIN);
    this->SetSizerAndFit(_sizer, false);

    Bind(wxEVT_BUTTON, &AttachmentPanelWithButton::onButtonClick, this);
}

void AttachmentPanelWithButton::onButtonClick(wxCommandEvent& e) {
    wxCommandEvent newEvent(ATTACHMENT_BUTTON_CLICK_EVENT);
    newEvent.SetInt(_index);
    wxPostEvent(GetEventHandler(), newEvent);
}