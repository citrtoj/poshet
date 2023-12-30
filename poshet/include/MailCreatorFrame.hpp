#pragma once

#include <wx/wx.h>
#include <wx/strconv.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtexthtml.h>
#include <wx/stream.h>
#include <wx/sstream.h>
#include <wx/richtext/richtextxml.h>

class MailCreatorFrame : public wxFrame {
protected:
    //MailCreatorFrameSubscriber* _subscriber;
    
    wxButton* _sendBtn, * _attachBtn;
    wxTextCtrl* _toInput, * _subjectInput;
    wxRichTextCtrl* _contentsCtrl;

    static const int MARGIN = 5;

    // wxWidgets event handlers
    void OnSend(wxCommandEvent& event);
    void OnClose(wxEvent& event);
public:
    MailCreatorFrame();
    //void subscribe(MailCreatorFrameSubscriber* sub);

    bool closeGracefully();

    std::string to() const;
    std::string subject() const;
    std::string body() const;

};