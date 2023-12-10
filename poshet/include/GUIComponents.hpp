#pragma once

#include <wx/wx.h>
#include "Mail.hpp"

class MailListItem : public wxPanel {
protected:
    wxStaticText* _from;
    wxStaticText* _subject;
public:
    MailListItem(wxWindow* parent, const MailInfo& info);
};