#include "GUIComponents.hpp"

wxDEFINE_EVENT(ATTACHMENT_BUTTON_CLICK_EVENT, wxCommandEvent);

AttachmentPanel::AttachmentPanel(wxWindow* parent, int index, const wxString& attachmentName, unsigned long long fileSize, int MARGIN) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED) {
    _sizer = new wxBoxSizer(wxHORIZONTAL);
    _text = new wxStaticText(this, wxID_ANY, attachmentLabel(attachmentName, fileSize));
    _sizer->Add(_text, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2 * MARGIN);
    this->SetSizerAndFit(_sizer);
    _attachmentName = attachmentName;
    _index = index;
    _fileSize = fileSize;
}
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

AlphaTextDialog::AlphaTextDialog(const wxString& title) : wxDialog(nullptr, wxID_ANY, title) {
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    auto validator = wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST | wxFILTER_ASCII);
    wxString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    validator.SetCharIncludes(validChars);
    _inputTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, validator);
    sizer->Add(_inputTextCtrl, 0, wxALL | wxEXPAND, 5);
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okButton = new wxButton(this, wxID_OK, "OK");
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
    buttonSizer->Add(okButton, 0, wxALL, 5);
    buttonSizer->Add(cancelButton, 0, wxALL, 5);
    sizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, 5);
    SetSizerAndFit(sizer);
}

wxString AlphaTextDialog::getInputString() const{
    return _inputTextCtrl->GetValue();
}