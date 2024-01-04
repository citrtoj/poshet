#include "GUIComponents.hpp"

wxDEFINE_EVENT(ATTACHMENT_BUTTON_CLICK_EVENT, wxCommandEvent);
wxDEFINE_EVENT(ATTACHMENT_CONTAINER_CLICK_EVENT, wxCommandEvent);

AttachmentPanel::AttachmentPanel(wxWindow* parent, int index, const wxString& attachmentName, unsigned long long fileSize, int MARGIN) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED) {
    SetBackgroundColour({255, 255, 255});
    _sizer = new wxBoxSizer(wxHORIZONTAL);
    _text = new wxStaticText(this, wxID_ANY, attachmentLabel(attachmentName, fileSize));
    _sizer->Add(_text, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2 * MARGIN);
    this->SetSizerAndFit(_sizer);
    _attachmentName = attachmentName;
    _index = index;
    _fileSize = fileSize;
}
wxString AttachmentPanel::attachmentLabel(const wxString& attachment, unsigned long long fileSize) {
    return wxString::Format("%-20s ~%s", attachment, Utils::fileSizeToString(fileSize));
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
    wxString validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_ ";
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

AttachmentsContainer::AttachmentsContainer(wxWindow* parent, long parentStyle, const wxString& attButtonName, int margin)
: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, parentStyle), _attButtonName(attButtonName), _margin(margin) {
    SetScrollRate(5, 0);
    _sizer = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(_sizer);
    Bind(ATTACHMENT_BUTTON_CLICK_EVENT, &AttachmentsContainer::OnAttachmentClick, this);
}

void AttachmentsContainer::addAttachment(const wxString& attachmentName, unsigned long long fileSize) {
    auto attBtn = new AttachmentPanelWithButton(this, _attachments.size(), attachmentName, fileSize, _attButtonName, _margin);
    _attachments.push_back(attBtn);
    _sizer->Add(attBtn, 0, wxALL, _margin);
}

void AttachmentsContainer::removeAttachment(size_t idx) {
    if (idx >= _attachments.size()) {
        throw Exception("No attachment at specified index");
    }
    if (_attachments.empty()) {
        return;
    }
    auto attachmentIterator = _attachments.begin() + idx;
    (*attachmentIterator)->Destroy();
    _attachments.erase(_attachments.begin() + idx);
    for (size_t i = idx; i < _attachments.size(); ++i) {
        _attachments[i]->setIndex(i);
    }
}

void AttachmentsContainer::clear() {
    for (auto x : _attachments) {
        x->Destroy();
    }
    _attachments.clear();
}

void AttachmentsContainer::OnAttachmentClick(wxCommandEvent& e) {
    wxCommandEvent newEvent(ATTACHMENT_CONTAINER_CLICK_EVENT);
    newEvent.SetInt(e.GetInt());
    wxPostEvent(GetEventHandler(), newEvent);
}