#pragma once

#include <wx/wx.h>
#include <wx/event.h>

#include "Utils.hpp"

enum {
    ID_ATTACHMENT_BUTTON = wxID_HIGHEST + 1
};


class AttachmentButton : public wxButton
{
public:
    AttachmentButton(wxWindow* parent, int index, const wxString& attachmentName, unsigned long long fileSize)
        : wxButton(parent, ID_ATTACHMENT_BUTTON, getButtonLabel(attachmentName, fileSize))
    {
        Bind(wxEVT_BUTTON, &AttachmentButton::OnAttachmentButtonClick, this);
        _attachmentName = attachmentName;
        _index = index;
        _fileSize = fileSize;
    }

private:
    void OnAttachmentButtonClick(wxCommandEvent& event);

    wxString getButtonLabel(const wxString& attachmentName, unsigned long long fileSize) {
        return wxString::Format("%-20s %20s", attachmentName, Utils::fileSizeToString(fileSize));
    }

    wxString _attachmentName;
    unsigned long long _fileSize;
    int _index;
};