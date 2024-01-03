#include "AppController.hpp"
AppController::AppController(wxApp* app, LoginFrame* loginFrame, DashboardFrame* dashboardFrame) :
    _mainApp(app),
    _loginFrame(loginFrame),
    _dashboardFrame(dashboardFrame)
{
    try {   
        _fileManager = new FileManager();
        _session = new Session(_fileManager);
    }
    catch (Exception& e) {
        showException(e.what());
    }

    _session->subscribe(this);

    // close-window binds
    _loginFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::onCloseApp, this);
    _dashboardFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::onCloseApp, this);

    _loginFrame->Bind(LOGIN_SUBMIT, &AppController::onLoginSubmit, this);

    _dashboardFrame->Bind(REFRESH_MAIL_LIST, &AppController::onRefreshMailList, this);
    _dashboardFrame->Bind(VIEW_MAIL_WITH_TAG, &AppController::onViewMailWithTag, this);
    _dashboardFrame->Bind(VIEW_ALL_MAIL, &AppController::onViewAllMail, this);
    
    _dashboardFrame->Bind(SELECT_MAIL, &AppController::onSelectMail, this);
    _dashboardFrame->Bind(TAG_MAIL, &AppController::onTagMail, this);
    _dashboardFrame->Bind(ATTACHMENT_DOWNLOAD, &AppController::onAttachmentDownload, this);
    _dashboardFrame->Bind(NEW_MAIL, &AppController::onNewMail, this);
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

void AppController::login() {
    try {
        _session->setLoginData(_loginFrame->userInput());
        _session->connectAndLoginToServers();
        getMailAndShow(true);
        _loginFrame->Hide();
    }
    catch (Exception& e) {
        showException(e.what());
        _session->closeConnections();
        return;
    }
}

void AppController::getMailAndShow(bool force) {
    try {
        _dashboardFrame->Show();
        _currentMail = _session->retrieveAllMail(force);
        auto tags = _session->mailTags();
        _dashboardFrame->setMailList(_currentMail);
        _dashboardFrame->setTags(tags);
    }
    catch (Exception& e) {
        showException(e.what());
        closeApp();
    }
}

// --- wxWidgets UI listeners ---

void AppController::onCloseApp(wxCloseEvent& e) {
    closeApp();
}

void AppController::onLoginSubmit(wxCommandEvent& e) {
    login();
}

void AppController::onRefreshMailList(wxCommandEvent& e) {
    getMailAndShow(true);
}

void AppController::onSelectMail(wxCommandEvent& e) {
    _selectedMail = _dashboardFrame->selected();
    const auto& mail = *(_currentMail[_selectedMail]);
    _dashboardFrame->updateViewMailPanel(mail);
}

bool AppController::createMailCreatorFrame() {
    if (_isMailCreatorOpen) {
        showInfo("A Mail Creator window is already open. Close the currently open one before proceeding.");
        return false;
    }
    _mailCreatorFrame = new MailCreatorFrame();

    _mailCreatorFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::onMailCreatorClose, this);
    _mailCreatorFrame->Bind(SEND_MAIL, &AppController::onMailCreatorSend, this);
    _mailCreatorFrame->Bind(ADD_ATTACHMENT, &AppController::onMailCreatorAddAttachment, this);
    _mailCreatorFrame->Bind(REMOVE_ATTACHMENT, &AppController::onMailCreatorRemoveAttachment, this);

    _mailCreatorFrame->Show();
    _isMailCreatorOpen = true;
    return true;
}

void AppController::onNewMail(wxCommandEvent& e) {
    if (!createMailCreatorFrame()) {
        return;
    }
    try {
        _mailBuilder = new MailBodyBuilder(_session->emailAddress(), _session->fullName());
    }
    catch (Exception& e) {
        showException(std::string("Could not initialize Mail Creator (") + e.what() + ").");
        delete _mailBuilder;
        _mailCreatorFrame->closeGracefully();
    }
}

void AppController::onReplyMail(wxCommandEvent& e) {
    if (!createMailCreatorFrame()) {
    return;
    }
    try {
        const auto& mail = _session->getMailAt(_selectedMail);
        _mailBuilder = new ReplyMailBodyBuilder(mail, _session->emailAddress(), _session->fullName());
        _mailCreatorFrame->updateAttachments(_mailBuilder->attachments());
        _mailCreatorFrame->setTo(_mailBuilder->to());
        _mailCreatorFrame->setSubject(_mailBuilder->subject());
        _mailCreatorFrame->setBody(_mailBuilder->generateStarterBody());
        _mailCreatorFrame->setBodyCursorToBeginning();
    }
    catch (Exception& e) {
        showException(std::string("Could not initialize Mail Creator (") + e.what() + ").");
        delete _mailBuilder;
        _mailCreatorFrame->closeGracefully();
    }
}

void AppController::onForwardMail(wxCommandEvent& e) {
    if (!createMailCreatorFrame()) {
        return;
    }
    try {
        const auto& mail = _session->getMailAt(_selectedMail);
        _mailBuilder = new ForwardMailBodyBuilder(mail, _session->emailAddress(), _session->fullName());
        _mailCreatorFrame->updateAttachments(_mailBuilder->attachments());
        _mailCreatorFrame->setSubject(_mailBuilder->subject());
        _mailCreatorFrame->setBody(_mailBuilder->generateStarterBody());
        _mailCreatorFrame->setBodyCursorToBeginning();
    }
    catch (Exception& e) {
        showException(std::string("Could not initialize Mail Creator (") + e.what() + ").");
        delete _mailBuilder;
        _mailCreatorFrame->closeGracefully();
    }
}

void AppController::onDeleteMail(wxCommandEvent& e) {
    wxMessageDialog dialog(nullptr, "Are you sure you want to delete this mail?", "Delete Confirmation", wxYES_NO | wxICON_QUESTION);

    int result = dialog.ShowModal();
    if (result != wxID_YES) {
        return;
    }

    auto selected = _dashboardFrame->selected();
    _session->deleteMail(selected);
    getMailAndShow();
}

void AppController::onMailCreatorSend(wxCommandEvent& e) {
    auto toInput = _mailCreatorFrame->to();
    auto subjectInput = _mailCreatorFrame->subject();
    auto bodyInput = _mailCreatorFrame->body();
    try {

        // add the info to the mail builder because that's what it'll truly use in the end
        // and send
        _mailBuilder->setTo(toInput);
        _mailBuilder->setSubject(subjectInput);
        _mailBuilder->setPlainText(bodyInput);

        // todo: create body with mailBuilder, send it to here, close the thing gracefully
        auto mailBody = _mailBuilder->generateMIMEMessage();
        auto from = _mailBuilder->fromEmailAddress();
        auto to = _mailBuilder->to();

        std::cout << mailBody <<"\n";
        
        _session->sendMail(from, to, mailBody);
        _mailCreatorFrame->closeGracefully();
    }
    catch (Exception& e) {
        showException(e.what());
    }
    showInfo("Mail sent successfully");
    getMailAndShow(true);
}

void AppController::onMailCreatorClose(wxCloseEvent& e) {
    _mailCreatorFrame->Destroy();
    _mailCreatorFrame = nullptr;
    _isMailCreatorOpen = false;
    delete _mailBuilder;
}

void AppController::onAttachmentDownload(wxCommandEvent& e) {
    auto attachmentIdx = e.GetInt();
    const auto& mail = _session->getMailAt(_selectedMail);
    auto attachmentData = mail.attachmentMetadataAt(e.GetInt());

    wxFileDialog saveFileDialog(nullptr, "Save As", wxEmptyString, wxEmptyString, wxT("All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    std::string filename = "";
    for (char ch : attachmentData._filename) {
        if (std::isalnum(ch)) {
            filename += ch;
        }
        else {
            filename += "_";
        }
    }
    saveFileDialog.SetFilename(filename);
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

void AppController::onTagMail(wxCommandEvent& e) {
    AlphaTextDialog dialog("Enter a tag");
    if (dialog.ShowModal() != wxID_OK) {
        return;
    }
    std::string userInput = dialog.getInputString().ToStdString();

    _selectedMail = _dashboardFrame->selected();
    _session->tagMail(_selectedMail, userInput);
    getMailAndShow();
}

void AppController::onViewMailWithTag(wxCommandEvent& e) {
    std::string tag = e.GetString().ToStdString();
    try {
        _currentMail = _session->retrieveMail(tag, false);
        _dashboardFrame->setMailList(_currentMail);
    }
    catch (Exception& e) {
        showException(e.what());
        closeApp();
    }
}

void AppController::onViewAllMail(wxCommandEvent& e) {
    try {
        _currentMail = _session->retrieveAllMail();
        _dashboardFrame->setMailList(_currentMail);
    }
    catch (Exception& e) {
        showException(e.what());
        closeApp();
    }
}

void AppController::onMailCreatorAddAttachment(wxCommandEvent& e) {
    try {
        wxFileDialog openFileDialog(_mailCreatorFrame, _("Open File"), wxEmptyString, wxEmptyString,
            _("All files (*.*)|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (openFileDialog.ShowModal() == wxID_CANCEL) {
            // User canceled the dialog
            wxMessageBox(_("Operation canceled."), _("Info"), wxOK | wxICON_INFORMATION);
            return;
        }
        std::string filePath = openFileDialog.GetPath().ToUTF8().data();
        std::string filename = openFileDialog.GetFilename().ToUTF8().data();

        std::string data = _fileManager->readFile(filePath);
        _mailBuilder->addAttachment(filename, data);
        handleMailBuilderDataUpdate();
    }
    catch(Exception& e) {
        showException(e.what());
    }
}

void AppController::onMailCreatorRemoveAttachment(wxCommandEvent& e) {
    _mailBuilder->removeAttachment(e.GetInt());
    handleMailBuilderDataUpdate();
}


AppController::~AppController() {
    // destroy business-logic
    delete _fileManager;
    delete _session;
}

void AppController::handleSessionDataUpdate() {
    getMailAndShow();
}

void AppController::handleMailBuilderDataUpdate() {
    // show info on mail creator frame
    if (_mailCreatorFrame == nullptr or _mailBuilder == nullptr) {
        throw Exception("Mail creator uninitialized");
    }
    _mailCreatorFrame->updateAttachments(_mailBuilder->attachments());
}