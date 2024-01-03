#include "DashboardFrame.hpp"

wxDEFINE_EVENT(SELECT_MAIL, wxCommandEvent);
wxDEFINE_EVENT(NEW_MAIL, wxCommandEvent);
wxDEFINE_EVENT(TAG_MAIL, wxCommandEvent);
wxDEFINE_EVENT(VIEW_MAIL_WITH_TAG, wxCommandEvent);
wxDEFINE_EVENT(VIEW_ALL_MAIL, wxCommandEvent);
wxDEFINE_EVENT(REPLY_MAIL, wxCommandEvent);
wxDEFINE_EVENT(FORWARD_MAIL, wxCommandEvent);
wxDEFINE_EVENT(DELETE_MAIL, wxCommandEvent);
wxDEFINE_EVENT(REFRESH_MAIL_LIST, wxCommandEvent);
wxDEFINE_EVENT(ATTACHMENT_DOWNLOAD, wxCommandEvent);

bool DashboardFrame::_isFSHandlerInit = false;

void DashboardFrame::initFSHandler() {
    if (_isFSHandlerInit) {
        return;
    }
    wxFileSystem::AddHandler(new wxMemoryFSHandler);
}

DashboardFrame::DashboardFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
    initFSHandler();
    Bind(wxEVT_SPLITTER_SASH_POS_CHANGED, &DashboardFrame::OnViewMailResize, this);

    _sidebarPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 600), wxBORDER_SUNKEN);
    auto sidebarPanelBoxSizer = new wxBoxSizer(wxVERTICAL);
    _newMailBtn = new wxButton(_sidebarPanel, wxID_ANY, "New Mail");
    _newMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnNewMail, this);
    _refreshMailBtn = new wxButton(_sidebarPanel, wxID_ANY, "Refresh mail");
    _refreshMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnRefreshMailList, this);
    _showAllMailBtn = new wxButton(_sidebarPanel, wxID_ANY, "View all mail");
    _showAllMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnViewAllMail, this);

    _tagList = new wxListBox(_sidebarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE);
    _tagList->Bind(wxEVT_LISTBOX, &DashboardFrame::OnViewTag, this);

    sidebarPanelBoxSizer->Add(_newMailBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL, MARGIN);
    sidebarPanelBoxSizer->Add(_refreshMailBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL, MARGIN);
    sidebarPanelBoxSizer->Add(_showAllMailBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL, MARGIN);
    sidebarPanelBoxSizer->Add(_tagList, 1, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL, MARGIN);
    _sidebarPanel->SetSizerAndFit(sidebarPanelBoxSizer);

    _splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(1200, 800));

    _mailList = new wxListView(_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_SUNKEN);
    
    for (int i = 0; i < _fields.size(); ++i) {
        _mailList->InsertColumn(i, _fields[i]);
    }
    _mailList->InsertColumn(_fields.size(), "Tag");

    _mailList->SetDoubleBuffered(true);
    this->Bind(wxEVT_LIST_ITEM_SELECTED, &DashboardFrame::OnListBoxEvent, this);

    wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(_sidebarPanel, 0, wxEXPAND | wxALL, MARGIN);
    mainSizer->Add(_splitter, 1, wxEXPAND | wxALL, MARGIN);
    _splitter->Initialize(_mailList);
    SetSizerAndFit(mainSizer);
    Centre();

    initViewMailPanel();
}

void DashboardFrame::initViewMailPanel() {
    if (_isViewMailPanelInit) {
        return;
    }
    _viewMailPanel = new wxPanel(_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    _viewMailPanel->SetBackgroundColour(wxColor(255, 255, 255));

    //_headerPanel = new wxPanel(_viewMailPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxEXPAND);
    auto headerSizer = new wxBoxSizer(wxHORIZONTAL);

    //headers sizer -- with To and Subject
    auto mailHeadersSizer = new wxBoxSizer(wxVERTICAL);
    _selectedMailFrom = new wxStaticText(_viewMailPanel, wxID_ANY, "Dummy From", wxDefaultPosition, {0, -1}, wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_END);
    _selectedMailTo = new wxStaticText(_viewMailPanel, wxID_ANY, "Dummy To", wxDefaultPosition, {0, -1}, wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_END);
    mailHeadersSizer->Add(_selectedMailFrom, 0, wxEXPAND | wxALL, MARGIN);
    mailHeadersSizer->Add(_selectedMailTo, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, MARGIN);

    auto mailButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    _tagMailBtn = new wxButton(_viewMailPanel, wxID_ANY, "Change tag");
    _tagMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnTagMail, this);
    mailButtonSizer->Add(_tagMailBtn, 1, wxALL, MARGIN);

    _replyMailBtn = new wxButton(_viewMailPanel, wxID_ANY, "Reply");
    _replyMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnReplyMail, this);
    mailButtonSizer->Add(_replyMailBtn, 1, wxALL, MARGIN);

    _forwardMailBtn = new wxButton(_viewMailPanel, wxID_ANY, "Forward");
    _forwardMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnForwardMail, this);
    mailButtonSizer->Add(_forwardMailBtn, 1, wxALL, MARGIN);

    _deleteMailBtn = new wxButton(_viewMailPanel, wxID_ANY, "Delete");
    _deleteMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnDeleteMail, this);
    mailButtonSizer->Add(_deleteMailBtn, 1, wxALL, MARGIN);

    headerSizer->Add(mailHeadersSizer, 1, wxEXPAND | wxALL, MARGIN);
    headerSizer->Add(mailButtonSizer, 0, wxALL, MARGIN);

    mailHeadersSizer->SetMinSize(mailButtonSizer->GetMinSize() / 2);

    auto mailSubjectSizer = new wxBoxSizer(wxVERTICAL);
    _selectedMailSubject = new wxStaticText(_viewMailPanel, wxID_ANY, "Dummy Subject", wxDefaultPosition, {0, -1}, wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_END);
    wxFont font = _selectedMailSubject->GetFont();
    font.SetPointSize(16);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    _selectedMailSubject->SetFont(font);

    mailSubjectSizer->Add(_selectedMailSubject, 0, wxEXPAND | wxALL, MARGIN);

    auto mailContentsPanel = new wxPanel(viewMailPanel(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    auto mailContentsSizer = new wxBoxSizer(wxVERTICAL);

    _plainTextDisplayer = new wxTextCtrl(mailContentsPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_WORDWRAP | wxTE_READONLY);
    _htmlDisplayer = new wxHtmlWindow(mailContentsPanel, wxID_ANY);
    _plainTextDisplayer->Hide();
    _htmlDisplayer->Hide();

    _displayer = new wxBoxSizer(wxVERTICAL);

    _displayer->Add(_htmlDisplayer, 1, wxEXPAND);
    _displayer->Add(_plainTextDisplayer, 1, wxEXPAND);

    mailContentsSizer->Add(_displayer, 1, wxEXPAND);
    mailContentsPanel->SetSizer(mailContentsSizer);

    _attContainer = new AttachmentsContainer(viewMailPanel(), wxBORDER_SUNKEN, "Save", MARGIN);
    _attContainer->Bind(ATTACHMENT_CONTAINER_CLICK_EVENT, &DashboardFrame::OnAttachmentDownload, this);

    // _mailAttachmentsPanel = new wxScrolledWindow(viewMailPanel(), wxID_ANY, wxDefaultPosition, {-1, -1}, wxBORDER_SUNKEN);
    // _mailAttachmentsPanel->Bind(ATTACHMENT_BUTTON_CLICK_EVENT, &DashboardFrame::OnAttachmentDownload, this);

    // _mailAttachmentsSizer = new wxBoxSizer(wxHORIZONTAL);
    // _mailAttachmentsPanel->SetSizer(_mailAttachmentsSizer);

    // _mailAttachmentsPanel->SetScrollRate(5, 0);

    _viewMailSizer = new wxBoxSizer(wxVERTICAL);
    _viewMailSizer->Add(headerSizer, 0, wxEXPAND);
    _viewMailSizer->Add(mailSubjectSizer, 0, wxEXPAND | wxALL, MARGIN);
    _viewMailSizer->Add(mailContentsPanel, 1, wxEXPAND | wxALL, MARGIN);
    _viewMailSizer->Add(_attContainer, 0, wxEXPAND | wxALL, MARGIN);
    _attContainer->Hide();

    _viewMailPanel->SetSizerAndFit(_viewMailSizer);
    _isViewMailPanelInit = true;
    refreshViewMailPanel();
}

void DashboardFrame::refreshViewMailPanel() {
    if (_isViewMailPanelInit) {
        _splitter->SplitVertically(_mailList, _viewMailPanel);
        _attContainer->refitSizer();
        _viewMailPanel->Layout();
        _splitter->Layout();
    }
}

void DashboardFrame::resetSash() {
    _splitter->SetSashPosition(_splitter->GetSize().GetWidth() - _viewMailPanel->GetMinSize().GetWidth());
}

// --- for memoryFSHandler ---

std::string DashboardFrame::newInlineAttachmentFilename() const {
    return "att_" + std::to_string(_savedToFSHandler.size());
}


// ---- wxWidgets-specific event handlers

void DashboardFrame::OnViewMailResize(wxSplitterEvent& e) {
    refreshViewMailPanel();
}

void DashboardFrame::OnListBoxEvent(wxCommandEvent& e) {
    wxCommandEvent newEvent(SELECT_MAIL);
    wxPostEvent(GetEventHandler(), newEvent);
}

void DashboardFrame::OnRefreshMailList(wxCommandEvent& e) {
    _tagList->SetSelection(wxNOT_FOUND);

    wxCommandEvent newEvent(REFRESH_MAIL_LIST);
    wxPostEvent(GetEventHandler(), newEvent);
    _splitter->SetSashPosition(_splitter->GetSize().GetWidth());
}

void DashboardFrame::OnViewTag(wxCommandEvent& e) {
    auto i = e.GetSelection();
    const auto& tag = _tags[i];
    
    wxCommandEvent newEvent(VIEW_MAIL_WITH_TAG);
    //newEvent.SetInt(i);
    newEvent.SetString(tag);
    wxPostEvent(GetEventHandler(), newEvent);
}

void DashboardFrame::OnViewAllMail(wxCommandEvent& e) {
    wxCommandEvent newEvent(VIEW_ALL_MAIL);
    wxPostEvent(GetEventHandler(), newEvent);
    _tagList->SetSelection(wxNOT_FOUND);
}

void DashboardFrame::OnNewMail(wxCommandEvent& e) {
    wxCommandEvent newEvent(NEW_MAIL);
    wxPostEvent(GetEventHandler(), newEvent);
}
void DashboardFrame::OnReplyMail(wxCommandEvent& e) {
    wxCommandEvent newEvent(REPLY_MAIL);
    wxPostEvent(GetEventHandler(), newEvent);
}
void DashboardFrame::OnForwardMail(wxCommandEvent& e) {
    wxCommandEvent newEvent(FORWARD_MAIL);
    wxPostEvent(GetEventHandler(), newEvent);
}

void DashboardFrame::OnDeleteMail(wxCommandEvent& e) {
    if (selected() < 0) {
        return;
    }
    wxCommandEvent newEvent(DELETE_MAIL);
    wxPostEvent(GetEventHandler(), newEvent);
}

void DashboardFrame::OnAttachmentDownload(wxCommandEvent& e) {
    wxCommandEvent newEvent(ATTACHMENT_DOWNLOAD);
    newEvent.SetInt(e.GetInt());
    wxPostEvent(GetEventHandler(), newEvent);
}


void DashboardFrame::OnTagMail(wxCommandEvent& e) {
    wxCommandEvent newEvent(TAG_MAIL);
    wxPostEvent(GetEventHandler(), newEvent);
}

// ---- internal getters ---

wxPanel* DashboardFrame::viewMailPanel() const {
    return _viewMailPanel;
}


// ---- external setters to be used by outside controllers

void DashboardFrame::setMailList(const std::vector<const Mail*>& mails) {
    _mailList->DeleteAllItems();
    for (const auto& mail : mails) {
        int index = _mailList->GetItemCount();
        wxListItem item;
        item.SetId(index);
        _mailList->InsertItem(item);

        for (int i = 0; i < _fields.size(); ++i) {
            auto fieldValue = mail->getHeaderField(_fields[i], true);
            _mailList->SetItem(index, i, wxString::FromUTF8(fieldValue.c_str()));
        }
        // tags will be ascii, a-zA-Z
        _mailList->SetItem(index, _fields.size(), wxString::FromAscii(mail->tag().c_str()));
    }
    for (int i = 0; i < _fields.size(); ++i) {
        _mailList->SetColumnWidth(i, wxLIST_AUTOSIZE);
        _mailList->Update();
    }
    _splitter->Unsplit(_viewMailPanel);
}

void DashboardFrame::updateViewMailPanel(const Mail& mail) {
    initViewMailPanel();
    _selectedMailFrom->SetLabel("From: " + mail.getHeaderField("From"));
    _selectedMailTo->SetLabel("To: " + mail.getHeaderField("To"));
    _selectedMailSubject->SetLabel(wxString::FromUTF8(mail.getHeaderField("Subject", true).c_str()));

    auto attachmentMetadata = mail.attachmentMetadata();
    if (attachmentMetadata.size() == 0) {
        _attContainer->Hide();
    }
    else {
        // clear attachments
        _attContainer->clear();
        for (size_t i = 0; i < attachmentMetadata.size(); ++i) {
            _attContainer->addAttachment(attachmentMetadata[i]._filename, attachmentMetadata[i]._size);
        }
        _attContainer->Show();
    }
    _savedToFSHandler.clear();
    
    try {
        auto htmlText = mail.getHTMLText();
        auto data = mail.getInlineHTMLAttachments();
        for (auto x : data) {
            size_t pos = 0;
            auto filename = newInlineAttachmentFilename();
            auto memoryPrefixedFilename = "memory:" + filename;
            while ((pos = htmlText.find(x._referenceId, pos)) != std::string::npos) {
                htmlText.replace(pos, x._referenceId.length(), memoryPrefixedFilename);
                pos += memoryPrefixedFilename.length();
            }
            wxMemoryFSHandler::AddFile(filename, x._data.c_str(), x._data.length());
            _savedToFSHandler.push_back(filename);
        }
        std::string htmlName = "file.htm";
        wxMemoryFSHandler::AddFile(htmlName, htmlText.c_str(), htmlText.length());
        _htmlDisplayer->Show();
        _htmlDisplayer->LoadPage("memory:" + htmlName);
        _plainTextDisplayer->Hide();
        wxMemoryFSHandler::RemoveFile(htmlName);
        for (auto file : _savedToFSHandler) {
            wxMemoryFSHandler::RemoveFile(file);
        }
        _savedToFSHandler.clear();

    }
    catch (MailException& e) {
        try {
            std::cout << "[DashboardFrame] Warning: " << e.what() << "\n";
            auto plainText = mail.getPlainTextPart();
            wxString x = wxString::FromUTF8(plainText.c_str());
            _plainTextDisplayer->SetValue(x);
            _plainTextDisplayer->Show();
            _htmlDisplayer->Hide();
        }
        catch (MailException& e) {
            std::cout << "[DashboardFrame] Warning: " << e.what() << "\n";
            wxString x = wxString::FromUTF8("Unfortunately, mail contents could not be displayed");
            _plainTextDisplayer->SetValue(x);
            _plainTextDisplayer->Show();
            _htmlDisplayer->Hide();
        }
    }
    refreshViewMailPanel();
}

long DashboardFrame::selected() const {
    return _mailList->GetFirstSelected();
}

void DashboardFrame::resetTags() {
    _tagList->Clear();
    for (int i = 0; i < _tags.size(); ++i) {
        if (!_tags[i].empty()) {
            _tagList->Insert(_tags[i], i);
        }
        else {
            _tagList->Insert(_displayUntaggedAs, i);
            //_tagList->SetSelection(i);
        }
    }
}

void DashboardFrame::setTags(const std::vector<std::string>& tags) {
    //_tagList->SetSelection(wxNOT_FOUND); //unselect tag
    _tags = tags;
    std::sort(_tags.begin(), _tags.end());
    resetTags();
}

std::string DashboardFrame::tagAt(size_t idx) {
    return _tags[idx];
}