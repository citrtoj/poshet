#pragma once

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/artprov.h>

#include "Utils.hpp"

wxDECLARE_EVENT(ATTACHMENT_BUTTON_CLICK_EVENT, wxCommandEvent);

class AttachmentPanel : public wxPanel {
public:
    AttachmentPanel(wxWindow* parent, int index, const wxString& attachmentName, unsigned long long fileSize, int MARGIN = 5);

protected:
    wxString attachmentLabel(const wxString& attachmentName, unsigned long long fileSize);
    wxBoxSizer* _sizer;
    wxStaticText* _text;
    wxString _attachmentName;
    unsigned long long _fileSize;
    int _index;
};

class AttachmentPanelWithButton : public AttachmentPanel {
public:
    AttachmentPanelWithButton(wxWindow* parent, int index, const wxString& attachmentName, unsigned long long fileSize, const wxString& buttonLabel, int MARGIN = 5);
protected:
    void onButtonClick(wxCommandEvent& e);

    wxButton* _downloadButton;
};


class AlphaTextDialog : public wxDialog {
public:
    AlphaTextDialog(const wxString& title);
    wxString getInputString() const;

private:
    wxTextCtrl* _inputTextCtrl;
};
