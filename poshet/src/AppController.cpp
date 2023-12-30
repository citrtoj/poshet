#include "AppController.hpp"
AppController::AppController(wxApp* app, LoginFrame* loginFrame, DashboardFrame* dashboardFrame) :
    _mainApp(app),
    _loginFrame(loginFrame),
    _dashboardFrame(dashboardFrame)
{
    try {
        _fileManager = new MailFileManager();
        _session = new Session(_fileManager);
    }
    catch (Exception& e) {
        showException(e.what());
    }

    _loginFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::onCloseApp, this);
    _dashboardFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::onCloseApp, this);

    _loginFrame->Bind(LOGIN_SUBMIT, &AppController::onLoginSubmit, this);

    _dashboardFrame->Bind(SELECT_MAIL, &AppController::onSelectMail, this);
    _dashboardFrame->Bind(ATTACHMENT_DOWNLOAD, &AppController::onAttachmentDownload, this);
    _dashboardFrame->Bind(REPLY_MAIL, &AppController::onReplyMail, this);
    _dashboardFrame->Bind(FORWARD_MAIL, &AppController::onForwardMail, this);
    _dashboardFrame->Bind(DELETE_MAIL, &AppController::onDeleteMail, this);

    _loginFrame->Show(true);
}

void AppController::showInfo(const std::string& msg) {
    wxMessageBox(msg, "Info", wxOK | wxICON_INFORMATION);
}

void AppController::showException(const std::string& msg) {
    wxMessageBox(msg, "Error", wxOK | wxICON_ERROR);
}

void AppController::warnUnimplemented() {
    showInfo("Feature unimplemented");
}

void AppController::closeApp() {
    wxCloseEvent close(wxEVT_CLOSE_WINDOW);
    wxPostEvent(_mainApp, close);
}

void AppController::onCloseApp(wxCloseEvent& e) {
    closeApp();
}

void AppController::onLoginSubmit(wxCommandEvent& e) {
    login();
}

void AppController::login() {
    try {
        _session->setLoginData(_loginFrame->userInput());
        _session->connectAndLoginToServers();
        _loginFrame->Hide();
        _dashboardFrame->Show();
        getSetMail(true);
    }
    catch (Exception& e) {
        showException(e.what());
        // todo: perhaps handle it or something... reset session?
        return;
    }
}

void AppController::onRefreshMailList(wxCommandEvent& e) {
    getSetMail(true);
}

void AppController::getSetMail(bool force) {
    try {
        _currentMail = _session->retrieveAllMail(force);
        _dashboardFrame->setMailList(_currentMail);
    }
    catch (Exception& e) {
        showException(e.what());
        closeApp();
    }
}

void AppController::onSelectMail(wxCommandEvent& e) {
    _selectedMail = _dashboardFrame->selected();
    const auto& mail = *(_currentMail[_selectedMail]);
    _dashboardFrame->updateViewMailPanel(mail);
}

void AppController::onNewMail(wxCommandEvent& e) {
    if (!_isMailCreatorOpen) {
        _mailCreatorFrame = new MailCreatorFrame();
        //_mailCreatorFrame->subscribe(this);
        _mailCreatorFrame->Show();
        _isMailCreatorOpen = true;
    }
}

void AppController::onReplyMail(wxCommandEvent& e) {
    warnUnimplemented();
}

void AppController::onForwardMail(wxCommandEvent& e) {
    warnUnimplemented();
}

void AppController::onDeleteMail(wxCommandEvent& e) {
    // ask for confirmation

    

    auto selected = _dashboardFrame->selected();
    _session->deleteMail(selected);
    getSetMail(true);
}

void AppController::onMailCreatorSend(wxCommandEvent& e) {
    auto to = _mailCreatorFrame->to();
    auto subject = _mailCreatorFrame->subject();
    auto body = _mailCreatorFrame->body();
    try {
        _session->sendMail(to, subject, body);
        _mailCreatorFrame->closeGracefully();
    }
    catch (Exception& e) {
        // show error
    }
    getSetMail(true);
}

void AppController::onMailCreatorClose(wxCommandEvent& e) {
    // todo: recheck this
    // shouldn't lead to bugs, despite it seemingly being a "i'm pretending it's done before it's actually done" situation
    // the GUI is singlethreaded, so immediately after this is set to false, the window of the mail creator frame receives the close event, and then that starts closing itself, more safely than I would have closed it manually using Destroy();
    // and only then would the user theoretically be even able to press the "new message" button again
    // perhaps in the future I might allow more than one window
    // but that would mean a more complicated subscriber system, I think
    // or the creator would have to send a pointer/index to itself... or something
    _isMailCreatorOpen = false;
}

void AppController::onAttachmentDownload(wxCommandEvent& e) {
    // _selectedMail is definitely positive otherwise there wouldn't be attachment download buttons lol
    auto attachmentIdx = e.GetInt();
    const auto& mail = _session->getMailAt(_selectedMail);
    auto attachmentData = mail.attachmentMetadataAt(e.GetInt());

    wxFileDialog saveFileDialog(nullptr, "Save As", wxEmptyString, wxEmptyString, wxT("All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    // todo: encode the slashes and whatnot from the data
    saveFileDialog.SetFilename(attachmentData._filename);

    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    auto attachment = mail.attachmentDataAt(e.GetInt());
    try {
        _fileManager->saveAttachment(saveFileDialog.GetPath().ToStdString(), attachment, true);
        showInfo("File saved successfully");
    }
    catch(FileManagerException& e) {
        showException(e.what());
    }
}

AppController::~AppController() {
    // destroy business-logic
    // don't delete UIs
    delete _fileManager;
    delete _session;
}