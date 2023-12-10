#include "DashboardFrame.hpp"

DashboardFrame::DashboardFrame(const wxString& title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_ANY, "&Quit\tCtrl-Q", "Quit Poshet");

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ANY, "&About\tF1", "Show about dialog");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to Poshet!");

    _sidebarPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 600), wxBORDER_SUNKEN);

    wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition,wxSize(400, -1));

    _mailList = new wxListView(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxEXPAND);
    _mailList->InsertColumn(0, "Name");
    _mailList->InsertColumn(1, "Address");
    _mailList->InsertColumn(2, "Subject");

    _viewMailWindow = new wxRichTextCtrl(splitter, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, -1), wxTE_MULTILINE | wxTE_READONLY | wxEXPAND);
    _viewMailWindow->Disable();

    wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(_sidebarPanel, 0, wxEXPAND | wxALL, 5);
    splitter->SplitVertically(_mailList, _viewMailWindow);
    mainSizer->Add(splitter, 1, wxEXPAND | wxALL, 5);
    // mainSizer->Add(_mailList, 1, wxEXPAND | wxALL, 5);
    // mainSizer->Add(_viewMailWindow, 2, wxEXPAND | wxALL, 5);

    SetSizerAndFit(mainSizer);
    Centre();
}

void DashboardFrame::setMail() {
    
}

void DashboardFrame::OnListBoxEvent(wxCommandEvent& e) {
    std::cout << e.GetSelection();
}