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

    bool _shouldClose = false;

    // wxWidgets event handlers
    void OnClose(wxEvent& event);
public:
    MailCreatorFrame();

    std::string to() const {
        return _toInput->GetValue().ToStdString();
    }

    void notifyToClose() {
        _shouldClose = true;
    }

    void subscribe(MailCreatorFrameSubscriber* sub);
};