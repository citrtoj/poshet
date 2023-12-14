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
    DashboardFrameSubscriber* _subscriber;

    static const int MARGIN = 5;

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
    wxRichTextCtrl* _mailContentsCtrl;
    wxButton* _replyMailBtn;
    wxButton* _forwardMailBtn;
    wxButton* _deleteMailBtn;

    // wxWidgets-specific event handlers
    void OnListBoxEvent(wxCommandEvent& e);
    void OnClose(wxEvent& e);
    void OnRefreshMailList(wxCommandEvent& e);
    void OnNewMail(wxCommandEvent& e);
    void OnDeleteMail(wxCommandEvent& e);


    // internal getters
    wxPanel* viewMailPanel() const;
    
    // internal functions
    void initViewMailPanel();
    void refreshViewMailPanel();

public:
    DashboardFrame(const wxString& title);

    // external setters to be used by outside controllers
    void subscribe(DashboardFrameSubscriber* sub);
    void setMailList(const std::vector<Mail>& mail);
    void updateViewMailPanel(const Mail& mail);

    // external getters
    long selected() const;
    
};