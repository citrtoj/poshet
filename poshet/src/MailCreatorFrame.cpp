#include "MailCreatorFrame.hpp"

MailCreatorFrame::MailCreatorFrame() :
    wxFrame(nullptr, wxID_ANY, "New mail")
{
    this->Bind(wxEVT_CLOSE_WINDOW, &MailCreatorFrame::OnClose, this);

    auto mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    mainPanel->SetBackgroundColour(wxColour(255, 255, 255, 255));
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    auto headerSizer = new wxGridSizer(2, 2, MARGIN, MARGIN);

    headerSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "To"), 0, wxALIGN_CENTER_VERTICAL | wxALL, MARGIN);
    _toInput = new wxTextCtrl(mainPanel, wxID_ANY);
    headerSizer->Add(_toInput, 1, wxEXPAND);

    headerSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Subject"), 0, wxALIGN_CENTER_VERTICAL | wxALL, MARGIN);
    _subjectInput = new wxTextCtrl(mainPanel, wxID_ANY);
    headerSizer->Add(_subjectInput, 1, wxEXPAND);
    mainSizer->Add(headerSizer);

    mainPanel->SetSizer(mainSizer);
} 

void MailCreatorFrame::OnClose(wxEvent& event) {
    _subscriber->onCloseMailCreator();
    if (_shouldClose) {
        event.Skip(); // should singlehandedly make the window simply close
    }
}

void MailCreatorFrame::subscribe(MailCreatorFrameSubscriber* sub) {
    _subscriber = sub;
}