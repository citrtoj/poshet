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
    void subscribe(MailCreatorFrameSubscriber* sub);

    bool closeGracefully();

    std::string to() const;
    std::string subject() const;
    std::string body() const;

};