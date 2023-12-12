#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/splitter.h>
#include <iostream>
#include <string>
#include <array>
#include <unordered_map>

#include "Mail.hpp"
#include "FrameSubscribers.hpp"

class DashboardFrame : public wxFrame {
protected:
    const int MARGIN = 5;
    DashboardFrameSubscriber* _subscriber;

    const std::vector<std::string> _fields = {
        "From", "Subject", "Date"
    };

    wxStatusBar* _statusBar;
    wxPanel* _sidebarPanel;
    wxButton* _newMailBtn;
    wxButton* _refreshMailBtn;
    wxListView* _mailList;
    wxSplitterWindow* _splitter;

    //Mail* currentMail;
    bool _isViewMailPanelInit = false;
    wxPanel* _viewMailPanel;
    wxPanel* _headerPanel;
    wxStaticText* _selectedMailFrom;
    wxStaticText* _selectedMailTo;
    wxStaticText* _selectedMailSubject;
    wxStaticText* _selectedMailDate;
    wxButton* _replyMailBtn;
    wxButton* _forwardMailBtn;
    wxButton* _deleteMailBtn;

    // wxWidgets-specific event handlers
    void OnListBoxEvent(wxCommandEvent& e);
    void OnClose(wxEvent& e);
    void OnRefresh(wxCommandEvent& e);
    void OnRefreshMailList(wxCommandEvent& e);
    void OnNewMail(wxCommandEvent& e);

    // internal getters
    wxPanel* viewMailPanel() const;
    
    // internal functions
    void initViewMailPanel();

public:
    DashboardFrame(const wxString& title);

    // external setters to be used by outside controllers
    void subscribe(DashboardFrameSubscriber* sub);
    void setMailList(const std::vector<Mail>& mail);
};