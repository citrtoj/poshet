#pragma once

#include <wx/wx.h>
#include <wx/strconv.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtexthtml.h>
#include <wx/stream.h>
#include <wx/sstream.h>
#include <wx/richtext/richtextxml.h>

wxDECLARE_EVENT(SEND_MAIL, wxCommandEvent);

class MailCreatorFrame : public wxFrame {
protected:
    //MailCreatorFrameSubscriber* _subscriber;
    
    wxButton* _sendBtn, * _attachBtn;
    wxTextCtrl* _toInput, * _subjectInput;
    wxRichTextCtrl* _contentsCtrl;

    static const int MARGIN = 5;

    // wxWidgets event handlers
    void OnSend(wxCommandEvent& event);
public:
    MailCreatorFrame();

    bool closeGracefully();

    std::string to() const;
    std::string subject() const;
    std::string body() const;

};