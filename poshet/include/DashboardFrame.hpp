#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <unordered_map>

#include <wx/wx.h>
#include <wx/wxhtml.h>
#include <wx/scrolwin.h>
#include <wx/wrapsizer.h>
#include <wx/listctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/splitter.h>
#include <wx/mstream.h>
#include <wx/wfstream.h>
#include <wx/base64.h>
#include <wx/fs_mem.h>
#include <mimetic/mimetic.h>

#include "Mail.hpp"
#include "GUIComponents.hpp"

wxDECLARE_EVENT(SELECT_MAIL, wxCommandEvent);
wxDECLARE_EVENT(NEW_MAIL, wxCommandEvent);
wxDECLARE_EVENT(REPLY_MAIL, wxCommandEvent);
wxDECLARE_EVENT(FORWARD_MAIL, wxCommandEvent);
wxDECLARE_EVENT(DELETE_MAIL, wxCommandEvent);
wxDECLARE_EVENT(REFRESH_MAIL_LIST, wxCommandEvent);
wxDECLARE_EVENT(ATTACHMENT_DOWNLOAD, wxCommandEvent);

class DashboardFrame : public wxFrame {
protected:
    //DashboardFrameSubscriber* _subscriber;

    static const int MARGIN = 5;

    const std::vector<std::string> _fields = {
        "From", "Subject", "Date"
    };

    wxStatusBar* _statusBar;
    wxPanel* _sidebarPanel;
    wxButton* _refreshMailBtn;
    wxButton* _newMailBtn;
    wxListBox* _folderList;
    wxListView* _mailList;
    wxSplitterWindow* _splitter;

    //Mail* currentMail;
    bool _isViewMailPanelInit = false;
    wxPanel* _viewMailPanel;
    wxBoxSizer* _viewMailSizer;
    wxPanel* _headerPanel;
    wxStaticText* _selectedMailFrom;
    wxStaticText* _selectedMailTo;
    wxStaticText* _selectedMailSubject;
    wxStaticText* _selectedMailDate;
    wxHtmlWindow* _htmlDisplayer;
    wxTextCtrl* _plainTextDisplayer;
    wxBoxSizer* _displayer;
    wxButton* _replyMailBtn;
    wxButton* _forwardMailBtn;
    wxButton* _deleteMailBtn;
    wxScrolledWindow* _mailAttachmentsPanel;
    wxBoxSizer* _mailAttachmentsSizer;

    // wxWidgets-specific event handlers
    void OnListBoxEvent(wxCommandEvent& e);
    void OnViewMailResize(wxSplitterEvent& e);
    void OnRefreshMailList(wxCommandEvent& e);
    void OnNewMail(wxCommandEvent& e);
    void OnReplyMail(wxCommandEvent& e);
    void OnForwardMail(wxCommandEvent& e);
    void OnDeleteMail(wxCommandEvent& e);
    void OnAttachmentDownload(wxCommandEvent& e);

    // internal getters
    wxPanel* viewMailPanel() const;
    
    // internal functions
    void initViewMailPanel();
    void refreshViewMailPanel();
    void resetSash();

    std::string memoryPrefix() const {
        return "___DASHBOARD_FRAME_";
    }

public:
    DashboardFrame(const wxString& title);

    // external setters to be used by outside controllers
    void setMailList(const std::vector<Mail>& mail);
    void updateViewMailPanel(const Mail& mail);

    // external getters
    long selected() const;
    
};