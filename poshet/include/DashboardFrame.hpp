#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <unordered_map>
#include <algorithm>

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
wxDECLARE_EVENT(TAG_MAIL, wxCommandEvent);
wxDECLARE_EVENT(REPLY_MAIL, wxCommandEvent);
wxDECLARE_EVENT(FORWARD_MAIL, wxCommandEvent);
wxDECLARE_EVENT(DELETE_MAIL, wxCommandEvent);
wxDECLARE_EVENT(REFRESH_MAIL_LIST, wxCommandEvent);
wxDECLARE_EVENT(ATTACHMENT_DOWNLOAD, wxCommandEvent);

class DashboardFrame : public wxFrame {
protected:
    static const int MARGIN = 5;


    const std::vector<std::string> _fields = {
        // they have to be actual fields that would be found in a mail header
        "From", "Subject", "Date"
    };

    wxStatusBar* _statusBar;
    wxPanel* _sidebarPanel;
    wxButton* _refreshMailBtn;
    wxButton* _newMailBtn;

    wxListBox* _tagList;
    std::string _defaultTagName = "All mail";

    std::vector<std::string> _tags;

    wxListView* _mailList;
    wxSplitterWindow* _splitter;

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
    wxButton* _tagMailBtn;
    wxButton* _replyMailBtn;
    wxButton* _forwardMailBtn;
    wxButton* _deleteMailBtn;
    wxScrolledWindow* _mailAttachmentsPanel;
    wxBoxSizer* _mailAttachmentsSizer;

    void OnListBoxEvent(wxCommandEvent& e);
    void OnViewMailResize(wxSplitterEvent& e);
    void OnRefreshMailList(wxCommandEvent& e);
    void OnNewMail(wxCommandEvent& e);
    void OnTagMail(wxCommandEvent& e);
    void OnReplyMail(wxCommandEvent& e);
    void OnForwardMail(wxCommandEvent& e);
    void OnDeleteMail(wxCommandEvent& e);
    void OnAttachmentDownload(wxCommandEvent& e);

    wxPanel* viewMailPanel() const;
    
    void initViewMailPanel();
    void refreshViewMailPanel();
    void resetSash();

    std::string memoryPrefix() const {
        // for wxMemoryFSHandler??
        return "___DASHBOARD_FRAME_";
    }

    void resetTags();

public:
    DashboardFrame(const wxString& title);

    // external setters to be used by outside controllers
    void setTags(const std::vector<std::string>& tags);
    void setMailList(const std::vector<const Mail*>& mail);
    void updateViewMailPanel(const Mail& mail);

    // external getters
    long selected() const;

    std::string tagAt(size_t idx);
    
};