#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/splitter.h>
#include <iostream>
#include <string>

#include "FrameSubscribers.hpp"

class DashboardFrame : public wxFrame {
protected:
    DashboardFrameSubscriber* _subscriber;

    wxPanel* _sidebarPanel;
    wxButton* _newMailBtn;
    wxListView* _mailList;
    wxRichTextCtrl* _viewMailWindow;
    wxStatusBar* _statusBar;
public:
    DashboardFrame(const wxString& title);
    void subscribe(DashboardFrameSubscriber* sub) {
        _subscriber = sub;
    }

    void OnListBoxEvent(wxCommandEvent& e);
    void OnClose(wxEvent& e);

    void setMail();
};