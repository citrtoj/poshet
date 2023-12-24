#include "DashboardFrame.hpp"

DashboardFrame::DashboardFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
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
    _folderList = new wxListBox(_sidebarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    // temporary, to demonstrate how it would look
    std::vector<std::string> dummyFolders = {
        "Dummy Folder 1",
        "Dummy Folder 2",
        "Dummy Folder 3"
    };
    for (int i = 0; i < dummyFolders.size(); ++i) {
        _folderList->Insert(dummyFolders[i], i);
    }

    sidebarPanelBoxSizer->Add(_newMailBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, MARGIN);
    sidebarPanelBoxSizer->Add(_refreshMailBtn, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, MARGIN);
    sidebarPanelBoxSizer->Add(_folderList, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, MARGIN);
    sidebarPanelBoxSizer->AddStretchSpacer(1);
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
}

void DashboardFrame::refreshViewMailPanel() {
    if (_isViewMailPanelInit) {
        _splitter->SplitVertically(_mailList, _viewMailPanel, 0);
        return;
    }
}

void DashboardFrame::initViewMailPanel() {
    if (_isViewMailPanelInit) {
        return;
    }
    _viewMailPanel = new wxPanel(_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    _viewMailPanel->SetBackgroundColour(wxColor(255, 255, 255));

    //_headerPanel = new wxPanel(_viewMailPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxEXPAND);
    auto headerSizer = new wxBoxSizer(wxHORIZONTAL);

    auto mailHeadersSizer = new wxBoxSizer(wxVERTICAL);
    _selectedMailFrom = new wxStaticText(_viewMailPanel, wxID_ANY, "Dummy Subject");
    _selectedMailTo = new wxStaticText(_viewMailPanel, wxID_ANY, "Dummy To");
    mailHeadersSizer->Add(_selectedMailFrom);
    mailHeadersSizer->Add(_selectedMailTo);

    auto mailButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    mailButtonSizer->AddStretchSpacer(); // "align right"
    _replyMailBtn = new wxButton(_viewMailPanel, wxID_ANY, "Reply");
    _forwardMailBtn = new wxButton(_viewMailPanel, wxID_ANY, "Forward");
    _deleteMailBtn = new wxButton(_viewMailPanel, wxID_ANY, "Delete");
    _deleteMailBtn->Bind(wxEVT_BUTTON, &DashboardFrame::OnDeleteMail, this);
    mailButtonSizer->Add(_replyMailBtn, 0, wxALL, MARGIN);
    mailButtonSizer->Add(_forwardMailBtn, 0, wxALL, MARGIN);
    mailButtonSizer->Add(_deleteMailBtn, 0, wxALL, MARGIN);

    headerSizer->Add(mailHeadersSizer, 1, wxALL, MARGIN);
    headerSizer->Add(mailButtonSizer, 1, wxALL, MARGIN);
    //headerSizer->Layout();

    auto mailContentsPanel = new wxPanel(viewMailPanel(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    auto mailContentsSizer = new wxBoxSizer(wxVERTICAL);

    _mailContentsCtrl = new wxHtmlWindow(mailContentsPanel, wxID_ANY);
    mailContentsSizer->Add(_mailContentsCtrl, 1, wxEXPAND);
    mailContentsPanel->SetSizer(mailContentsSizer);

    //auto mailAttachmentsPanel = new wxPanel(viewMailPanel(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);

    _viewMailSizer = new wxBoxSizer(wxVERTICAL);
    _viewMailSizer->Add(headerSizer, 0, wxEXPAND | wxALL, MARGIN);
    //_viewMailPanel->SetMinSize({ headerSizer->GetSize().GetX(), _viewMailPanel->GetSize().GetY()});
    _viewMailSizer->Add(mailContentsPanel, 1, wxEXPAND | wxALL, MARGIN);

    //mainSizer->Add(mailAttachmentsPanel, 0, 0);

    _viewMailPanel->SetSizerAndFit(_viewMailSizer);

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
            auto fieldValue = mail.getHeaderField(_fields[i]);
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
    // PROTOTYPE: testing the wxHtmlWindow container and wxMemoryFSHandler
    initViewMailPanel();

    _selectedMailFrom->SetLabel("From: " + mail.getHeaderField("From"));
    _selectedMailTo->SetLabel("To: " + mail.getHeaderField("To"));

    wxString base64ImageData = "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";
    auto x = wxBase64Decode(base64ImageData, strlen(base64ImageData), wxBase64DecodeMode_Strict, 0);
    wxMemoryInputStream inputStream(x.GetData(), x.GetDataLen());
    wxImage image(inputStream, wxBITMAP_TYPE_GIF);
    wxBitmap bitmap(image);
    wxFileSystem::AddHandler(new wxMemoryFSHandler);
    wxMemoryFSHandler::AddFile(memoryPrefix() + "red_dot.gif", image, wxBITMAP_TYPE_GIF);
    std::string htmlText = "<div><p><center>Taken from the internet: <img src=\"memory:" + memoryPrefix() + "red_dot.gif\" alt=\"Red dot\" /><a href=\"https://github.com/citrtoj\">hiiiiiii!!!!!!!!</a></center></p></div>";
    wxMemoryFSHandler::AddFile(memoryPrefix() + "file.html", htmlText.c_str());
    _mailContentsCtrl->LoadPage("memory:" + memoryPrefix() + "file.html");
    refreshViewMailPanel();
    wxMemoryFSHandler::RemoveFile(memoryPrefix() + "red_dot.gif");
    wxMemoryFSHandler::RemoveFile(memoryPrefix() + "file.html");
}

long DashboardFrame::selected() const {
    return _mailList->GetFirstSelected();
}