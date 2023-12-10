#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/splitter.h>
#include <iostream>
#include <string>
#include "Session.hpp"

enum
{
    ID_Quit = 1,
    ID_About,
};

class DashboardFrame : public wxFrame {
protected:
    wxPanel* _sidebarPanel;
    wxListView* _mailList;
    wxRichTextCtrl* _viewMailWindow;
public:
    DashboardFrame(const wxString& title);
    void OnListBoxEvent(wxCommandEvent& e);
    void setMail();
};