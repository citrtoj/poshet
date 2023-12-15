#include "DashboardFrame.hpp"

DashboardFrame::DashboardFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1200, 800))
{
    SetDoubleBuffered(true);
    this->Bind(wxEVT_CLOSE_WINDOW, &DashboardFrame::OnClose, this);

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

    sidebarPanelBoxSizer->Add(_newMailBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, MARGIN);
    sidebarPanelBoxSizer->Add(_refreshMailBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxBOTTOM, MARGIN);
    _sidebarPanel->SetSizer(sidebarPanelBoxSizer);

    _splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition,wxSize(600, -1));

    _mailList = new wxListView(_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_SUNKEN);
    
    for (int i = 0; i < _fields.size(); ++i) {
        _mailList->InsertColumn(i, _fields[i]);
    }
    _mailList->SetDoubleBuffered(true);
    this->Bind(wxEVT_LIST_ITEM_SELECTED, &DashboardFrame::OnListBoxEvent, this);

    wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(_sidebarPanel, 0, wxEXPAND | wxALL, MARGIN);
    mainSizer->Add(_splitter, 4, wxEXPAND | wxALL, MARGIN);
    _splitter->Initialize(_mailList);
    SetSizerAndFit(mainSizer);
    Centre();
}

void DashboardFrame::refreshViewMailPanel() {
    if (_isViewMailPanelInit) {
        _viewMailPanel->Fit();
        _viewMailPanel->Layout();
        _splitter->SplitVertically(_mailList, _viewMailPanel, 0);
        return;
    }
}

void DashboardFrame::initViewMailPanel() {
    if (_isViewMailPanelInit) {
        return;
    }
    _viewMailPanel = new wxPanel(_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    _headerPanel = new wxPanel(viewMailPanel(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxEXPAND);
    _headerPanel->SetBackgroundColour(wxColor(255, 255, 255));
    auto headerSizer = new wxBoxSizer(wxHORIZONTAL);

    auto mailHeadersSizer = new wxBoxSizer(wxVERTICAL);
    _selectedMailFrom = new wxStaticText(_headerPanel, wxID_ANY, "Dummy Subject");
    _selectedMailTo = new wxStaticText(_headerPanel, wxID_ANY, "Dummy To");
    mailHeadersSizer->Add(_selectedMailFrom);
    mailHeadersSizer->Add(_selectedMailTo);

    auto mailButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    mailButtonSizer->AddStretchSpacer(); // "align right"
    _replyMailBtn = new wxButton(_headerPanel, wxID_ANY, "Reply");
    _forwardMailBtn = new wxButton(_headerPanel, wxID_ANY, "Forward");
    _deleteMailBtn = new wxButton(_headerPanel, wxID_ANY, "Delete");
    _deleteMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnDeleteMail, this);
    mailButtonSizer->Add(_replyMailBtn, 0, wxALL, MARGIN);
    mailButtonSizer->Add(_forwardMailBtn, 0, wxALL, MARGIN);
    mailButtonSizer->Add(_deleteMailBtn, 0, wxALL, MARGIN);

    headerSizer->Add(mailHeadersSizer, 0);
    headerSizer->Add(mailButtonSizer, 1);

    _headerPanel->SetSizerAndFit(headerSizer);

    auto mailContentsPanel = new wxPanel(viewMailPanel(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    auto mailContentsSizer = new wxBoxSizer(wxVERTICAL);

    _mailContentsCtrl = new wxRichTextCtrl(mailContentsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, 400), wxTE_READONLY);
    mailContentsSizer->Add(_mailContentsCtrl, wxEXPAND);
    mailContentsPanel->SetSizer(mailContentsSizer);

    //auto mailAttachmentsPanel = new wxPanel(viewMailPanel(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(_headerPanel, 0, wxEXPAND);
    mainSizer->Add(mailContentsPanel, 0, wxEXPAND);
    //mainSizer->Add(mailAttachmentsPanel, 0, 0);

    viewMailPanel()->SetSizer(mainSizer);
    viewMailPanel()->SetMinSize(wxSize(headerSizer->GetSize().GetX(), -1));

    refreshViewMailPanel();

    _isViewMailPanelInit = true;
}


// ---- wxWidgets-specific event handlers

void DashboardFrame::OnListBoxEvent(wxCommandEvent& e) {
    //std::cout << _mailList->GetFirstSelected() << "\n";
    _subscriber->onSelectMail();
    initViewMailPanel();
}

void DashboardFrame::OnClose(wxEvent& e) {
    _subscriber->onCloseApp();
}

void DashboardFrame::OnRefreshMailList(wxCommandEvent& e) {
    _subscriber->onRefreshMailList();
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
            auto it = mail.headers().find(_fields[i]);
            std::string fieldValue = "";
            if (it != mail.headers().cend()) {
                fieldValue = it->second;
            }
            _mailList->SetItem(index, i, fieldValue);
        }
    }
    for (int i = 0; i < _fields.size(); ++i) {
        _mailList->SetColumnWidth(i, wxLIST_AUTOSIZE);
        _mailList->Update();
    }
    _splitter->Unsplit(_viewMailPanel);

}

void DashboardFrame::updateViewMailPanel(const Mail& mail) {
    // not final in any way shape or form
    initViewMailPanel();
    _selectedMailFrom->SetLabel("From: " + mail.getHeader("From"));
    _selectedMailTo->SetLabel("To: " + mail.getHeader("To"));
    _mailContentsCtrl->SetValue(mail.plainText());
    refreshViewMailPanel();
}

long DashboardFrame::selected() const {
    return _mailList->GetFirstSelected();
}