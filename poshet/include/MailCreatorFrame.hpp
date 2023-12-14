#pragma once
#include <wx/wx.h>
#include <wx/strconv.h>
#include <wx/richtext/richtextctrl.h>
#include "FrameSubscribers.hpp"

class MailCreatorFrame : public wxFrame {
protected:
    MailCreatorFrameSubscriber* _subscriber;
    wxButton* _sendBtn, * _attachBtn;
    wxTextCtrl* _toInput, * _subjectInput;
    wxRichTextCtrl* _contentsCtrl;

    static const int MARGIN = 5;

    // wxWidgets event handlers
    void OnSend(wxCommandEvent& event);
    void OnClose(wxEvent& event);
public:
    MailCreatorFrame();

    std::string to() const {
        return _toInput->GetValue().ToStdString();
    }
    std::string subject() const {
        return _subjectInput->GetValue().ToStdString();
    }

    std::string body() const {
        return _contentsCtrl->GetValue().ToStdString();
    }

    void subscribe(MailCreatorFrameSubscriber* sub);
};