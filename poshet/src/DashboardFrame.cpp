#include "DashboardFrame.hpp"

DashboardFrame::DashboardFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
    SetDoubleBuffered(true);
    this->Bind(wxEVT_CLOSE_WINDOW, &DashboardFrame::OnClose, this);
    this->Bind(wxEVT_SPLITTER_SASH_POS_CHANGED, &DashboardFrame::OnViewMailResize, this);

    //top menu... not sure if I'll truly need it but...
    auto *menuFile = new wxMenu;
    menuFile->Append(wxID_ANY, "&Quit\tCtrl-Q", "Quit Poshet");
    auto *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ANY, "&About\tF1", "Show about dialog");
    auto *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);

    _sidebarPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 600), wxBORDER_SUNKEN);
    auto sidebarPanelBoxSizer = new wxBoxSizer(wxVERTICAL);
    _newMailBtn = new wxButton(_sidebarPanel, wxID_ANY, "Refresh mail list");
    _newMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnRefreshMailList, this);
    _refreshMailBtn = new wxButton(_sidebarPanel, wxID_ANY, "New message");
    _refreshMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnNewMail, this);
    _folderList = new wxListBox(_sidebarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    // temporary, to demonstrate how it would look
    std::vector<std::string> dummyFolders = {
        "Inbox",
    };
    for (int i = 0; i < dummyFolders.size(); ++i) {
        _folderList->Insert(dummyFolders[i], i);
    }

    sidebarPanelBoxSizer->Add(_newMailBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL, MARGIN);
    sidebarPanelBoxSizer->Add(_refreshMailBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL, MARGIN);
    sidebarPanelBoxSizer->Add(_folderList, 1, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL, MARGIN);
    _sidebarPanel->SetSizerAndFit(sidebarPanelBoxSizer);

    _splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(1200, 800));

    _mailList = new wxListView(_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_SUNKEN);
    
    for (int i = 0; i < _fields.size(); ++i) {
        _mailList->InsertColumn(i, _fields[i]);
    }
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
    _replyMailBtn = new wxButton(_viewMailPanel, wxID_ANY, "Reply");
    _forwardMailBtn = new wxButton(_viewMailPanel, wxID_ANY, "Forward");
    _deleteMailBtn = new wxButton(_viewMailPanel, wxID_ANY, "Delete");
    
    _deleteMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnDeleteMail, this);
    mailButtonSizer->Add(_replyMailBtn, 1, wxALL, MARGIN);
    mailButtonSizer->Add(_forwardMailBtn, 1, wxALL, MARGIN);
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

    _viewMailSizer = new wxBoxSizer(wxVERTICAL);
    _viewMailSizer->Add(headerSizer, 0, wxEXPAND);
    _viewMailSizer->Add(mailSubjectSizer, 0, wxEXPAND | wxALL, MARGIN);
    _viewMailSizer->Add(mailContentsPanel, 1, wxEXPAND | wxALL, MARGIN);

    _viewMailPanel->SetSizerAndFit(_viewMailSizer);
    _isViewMailPanelInit = true;
    refreshViewMailPanel();
}

void DashboardFrame::refreshViewMailPanel() {
    if (_isViewMailPanelInit) {
        _splitter->SplitVertically(_mailList, _viewMailPanel);
        _viewMailPanel->Layout();
    }
}

void DashboardFrame::resetSash() {
    _splitter->SetSashPosition(_splitter->GetSize().GetWidth() - _viewMailPanel->GetMinSize().GetWidth());
}

// ---- wxWidgets-specific event handlers

void DashboardFrame::OnViewMailResize(wxSplitterEvent& e) {
    refreshViewMailPanel();
}

void DashboardFrame::OnListBoxEvent(wxCommandEvent& e) {
    _subscriber->onSelectMail();
}

void DashboardFrame::OnClose(wxEvent& e) {
    _subscriber->onCloseApp();
}

void DashboardFrame::OnRefreshMailList(wxCommandEvent& e) {
    _subscriber->onRefreshMailList();
    // hide sash
    _splitter->SetSashPosition(_splitter->GetSize().GetWidth());
}

void DashboardFrame::OnNewMail(wxCommandEvent& e) {
    _subscriber->onNewMail();
}

void DashboardFrame::OnDeleteMail(wxCommandEvent& e) {
    if (selected() < 0) {
        return;
    }
    _subscriber->onDeleteMail();
}


// ---- internal getters ---

wxPanel* DashboardFrame::viewMailPanel() const {
    return _viewMailPanel;
}


// ---- external setters to be used by outside controllers

void DashboardFrame::subscribe(DashboardFrameSubscriber* sub) {
    _subscriber = sub;
}

void DashboardFrame::setMailList(const std::vector<Mail>& mails) {
    _mailList->DeleteAllItems();
    for (const auto& mail : mails) {
        int index = _mailList->GetItemCount();
        wxListItem item;
        item.SetId(index);
        _mailList->InsertItem(item);

        for (int i = 0; i < _fields.size(); ++i) {
            auto fieldValue = mail.getHeaderField(_fields[i], true);
            _mailList->SetItem(index, i, wxString::FromUTF8(fieldValue.c_str()));
        }
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

    try {
        auto htmlText = mail.getHTMLPart();
        _htmlDisplayer->SetPage(htmlText);
        _plainTextDisplayer->Hide();
        _htmlDisplayer->Show();
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