#include "DashboardFrame.hpp"

DashboardFrame::DashboardFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
    this->Bind(wxEVT_CLOSE_WINDOW, &DashboardFrame::OnClose, this);

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_ANY, "&Quit\tCtrl-Q", "Quit Poshet");
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ANY, "&About\tF1", "Show about dialog");
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);

    _sidebarPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 600), wxBORDER_SUNKEN | wxVERTICAL);
    _newMailBtn = new wxButton(_sidebarPanel, wxID_ANY, "New mail", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);

    wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition,wxSize(600, -1));
    _mailList = new wxListView(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    _mailList->InsertColumn(0, "Name");
    _mailList->InsertColumn(1, "Address");
    _mailList->InsertColumn(2, "Subject");
    _viewMailWindow = new wxRichTextCtrl(splitter, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    _viewMailWindow->Disable();
    wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(_sidebarPanel, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(splitter, 4, wxEXPAND | wxALL, 5);
    splitter->SplitVertically(_mailList, _viewMailWindow, 0);
    SetSizerAndFit(mainSizer);
    Centre();
}

void DashboardFrame::setMail() {
    
}

void DashboardFrame::OnListBoxEvent(wxCommandEvent& e) {
    std::cout << e.GetSelection();
}

void DashboardFrame::OnClose(wxEvent& e) {
    _subscriber->onCloseAnyWindow();
}