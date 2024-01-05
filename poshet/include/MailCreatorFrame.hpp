#pragma once

#include <wx/wx.h>
#include <wx/strconv.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtexthtml.h>
#include <wx/stream.h>
#include <wx/sstream.h>
#include <wx/richtext/richtextxml.h>

#include "Mail.hpp"
#include "GUIComponents.hpp"

wxDECLARE_EVENT(SEND_MAIL, wxCommandEvent);
wxDECLARE_EVENT(ADD_ATTACHMENT, wxCommandEvent);
wxDECLARE_EVENT(REMOVE_ATTACHMENT, wxCommandEvent);

class MailCreatorFrame : public wxFrame {
protected:
    wxPanel* _mainPanel;
    wxButton* _sendBtn, * _attachBtn;
    wxTextCtrl* _toInput, * _subjectInput;
    wxRichTextCtrl* _contentsCtrl;
    AttachmentsContainer* _attContainer;

    static const int MARGIN = 5;
public:
    MailCreatorFrame();

    bool closeGracefully();

    std::string to() const;
    std::string subject() const;
    std::string body() const;

    void setTo(const std::string& text);
    void setSubject(const std::string& text);
    void setBody(const std::string& text);

    void setBodyCursorToBeginning();

    void updateAttachments(const std::vector<AttachmentMetadata>& metadata);

    void toggleAttachments(wxCommandEvent& e);
    bool _isAttContainerShowing = false;

    void OnSend(wxCommandEvent& event);
    void OnAttach(wxCommandEvent& event);
    void OnRemove(wxCommandEvent& event);
};