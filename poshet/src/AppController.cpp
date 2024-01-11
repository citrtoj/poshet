#include "AppController.hpp"
AppController::AppController(wxApp* app, LoginFrame* loginFrame, DashboardFrame* dashboardFrame, UsersFrame* usersFrame) :
    _mainApp(app),
    _loginFrame(loginFrame),
    _dashboardFrame(dashboardFrame),
    _usersFrame(usersFrame)
{
    try {   
        _fileManager = new FileManager();
        _session = new Session(_fileManager);
        _usersManager = new UsersManager(_fileManager);
    }
    catch (Exception& e) {
        showException(e.what());
    }

    _session->subscribe(this);

    _loginFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::onLoginClose, this);
    _dashboardFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::onCloseApp, this);
    _usersFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::onCloseApp, this);


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

    _usersFrame->Bind(LOGIN_USER, &AppController::onLogin, this);
    _usersFrame->Bind(ADD_USER, &AppController::onUsersAdd, this);
    _usersFrame->Bind(DELETE_USER, &AppController::onUsersDelete, this);
    _usersFrame->Bind(EDIT_USER, &AppController::onUsersEdit, this);

    updateUserFrame();
}

void AppController::updateUserFrame() {
    _usersFrame->setUsers(_usersManager->users());
    _usersFrame->Show();
}

void AppController::showInfo(const std::string& msg) {
    wxMessageBox(msg, "Info", wxOK | wxICON_INFORMATION);
}

void AppController::showException(const std::string& msg) {
    wxMessageBox(msg, "Error", wxOK | wxICON_ERROR);
}

void AppController::closeApp() {
    wxCloseEvent close(wxEVT_CLOSE_WINDOW);
    wxPostEvent(_mainApp, close);
}

void AppController::login() {
    try {
        _session->closeConnections();
        _session->setLoginData(selectedUserData());
        _session->connectAndLoginToServers();
        getMailAndShow(true);
        _usersFrame->Hide();
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
        _dashboardFrame->setTitleAddr(_session->emailAddress());
        _dashboardFrame->showMessage("Retrieving mail from database and POP3 mailbox");
        _currentMail = _session->retrieveAllMail(force);
        _dashboardFrame->showMessage("Retrieving mail tags");
        auto tags = _session->mailTags();
        _dashboardFrame->setMailList(_currentMail);
        _dashboardFrame->setTags(tags);
        _dashboardFrame->showMessage("Ready");
    }
    catch (IOException& e) {
        showException(e.what());
        try {
            _session->resetConnections();
        }
        catch (Exception& e) {
            showException(e.what());
            closeApp();
        }
    }
    catch (Exception& e) {
        showException(e.what());
        closeApp();
    }
}

void AppController::onCloseApp(wxCloseEvent& e) {
    closeApp();
}

void AppController::onLogin(wxCommandEvent& e) {
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

void AppController::initNewMailCreatorFrame() {
    _mailBuilder = new MailBodyBuilder(_session->emailAddress(), _session->fullName());
}

void AppController::onNewMail(wxCommandEvent& e) {
    if (!createMailCreatorFrame()) {
        return;
    }
    try {
        initNewMailCreatorFrame();
    }
    catch (Exception& e) {
        showException(std::string("Could not initialize Mail Creator (") + e.what() + ").");
        closeMailCreator();
    }
}

void AppController::initReplyMailCreatorFrame() {
    const auto& mail = _session->getMailAt(_selectedMail);
    _mailBuilder = new ReplyMailBodyBuilder(mail, _session->emailAddress(), _session->fullName());
    _mailCreatorFrame->updateAttachments(_mailBuilder->attachments());
    _mailCreatorFrame->setTo(_mailBuilder->to());
    _mailCreatorFrame->setSubject(_mailBuilder->subject());
    _mailCreatorFrame->setBody(_mailBuilder->generateStarterBody());
    _mailCreatorFrame->setBodyCursorToBeginning();
}

void AppController::onReplyMail(wxCommandEvent& e) {
    if (!createMailCreatorFrame()) {
        return;
    }
    try {
        initReplyMailCreatorFrame();
    }
    catch (Exception& e) {
        showException(std::string("Could not initialize Mail Creator (") + e.what() + ").");
        closeMailCreator();
    }
}

void AppController::initForwardMailCreatorFrame() {
    const auto& mail = _session->getMailAt(_selectedMail);
    _mailBuilder = new ForwardMailBodyBuilder(mail, _session->emailAddress(), _session->fullName());
    _mailCreatorFrame->updateAttachments(_mailBuilder->attachments());
    _mailCreatorFrame->setSubject(_mailBuilder->subject());
    _mailCreatorFrame->setBody(_mailBuilder->generateStarterBody());
    _mailCreatorFrame->setBodyCursorToBeginning();
}

void AppController::onForwardMail(wxCommandEvent& e) {
    if (!createMailCreatorFrame()) {
        return;
    }
    try {
        initForwardMailCreatorFrame();
    }
    catch (Exception& e) {
        showException(std::string("Could not initialize Mail Creator (") + e.what() + ").");
        closeMailCreator();
    }
}

void AppController::onDeleteMail(wxCommandEvent& e) {
    wxMessageDialog dialog(nullptr, "Are you sure you want to delete this mail?", "Delete Confirmation", wxYES_NO | wxICON_QUESTION);

    int result = dialog.ShowModal();
    if (result != wxID_YES) {
        return;
    }

    auto selected = _dashboardFrame->selected();
    try {
         _session->deleteMail(selected);
        getMailAndShow();
    }
    catch (IOException& e) {
        showException(e.what());
        try {
            _session->resetConnections();
        }
        catch (Exception& e) {
            showException(e.what());
            closeApp();
        }
    }
    catch (Exception& e) {
        showException(e.what());
        closeApp();
    }
}

void AppController::onMailCreatorSend(wxCommandEvent& e) {
    auto toInput = _mailCreatorFrame->to();
    auto subjectInput = _mailCreatorFrame->subject();
    auto bodyInput = _mailCreatorFrame->body();
    try {
        _mailBuilder->setTo(toInput);
        _mailBuilder->setSubject(subjectInput);
        _mailBuilder->setPlainText(bodyInput);

        auto mailBody = _mailBuilder->generateMIMEMessage();
        auto from = _mailBuilder->fromEmailAddress();
        auto to = _mailBuilder->to();
        
        try {
            _session->sendMail(from, to, mailBody);
        }
        catch (IOException& e) {
            showException(e.what());
            try {
                _session->resetConnections();
            }
            catch (Exception& e) {
                showException(e.what());
                closeApp();
            }
        }
        showInfo("Mail sent successfully");
        closeMailCreator();
    }
    catch (Exception& e) {
        showException(e.what());
        closeMailCreator();
        return;
    }
}

void AppController::closeMailCreator() {
    if (_isMailCreatorOpen) {
        _mailCreatorFrame->Destroy();
        _mailCreatorFrame = nullptr;
        _isMailCreatorOpen = false;
    }
    if (_mailBuilder != nullptr) {
        delete _mailBuilder;
        _mailBuilder = nullptr;
    }
}

void AppController::onMailCreatorClose(wxCloseEvent& e) {
    closeMailCreator();
}

void AppController::onAttachmentDownload(wxCommandEvent& e) {
    auto attachmentIdx = e.GetInt();
    const auto& mail = _session->getMailAt(_selectedMail);
    auto attachmentData = mail.attachmentMetadataAt(e.GetInt());

    wxFileDialog saveFileDialog(nullptr, "Save As", wxEmptyString, wxEmptyString, wxT("All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    std::string filename = "";
    for (char ch : attachmentData._filename) {
        if (std::isalnum(ch) or (ch == '.')) {
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
    try {
        _session->tagMail(_selectedMail, userInput);
    }
    catch (Exception& e) {
        showException(e.what());
        closeApp();
    }
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
        closeMailCreator();
    }
}

void AppController::onMailCreatorRemoveAttachment(wxCommandEvent& e) {
    _mailBuilder->removeAttachment(e.GetInt());
    handleMailBuilderDataUpdate();
}


AppController::~AppController() {
    // destroy business-logic
    closeMailCreator();
    delete _session;
    delete _usersManager;
    delete _fileManager;
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

void AppController::onUsersAdd(wxCommandEvent& e) {
    _loginFrame->Unbind(LOGIN_SUBMIT, &AppController::onLoginAdd, this);
    _loginFrame->Unbind(LOGIN_SUBMIT, &AppController::onLoginEdit, this);
    _loginFrame->Bind(LOGIN_SUBMIT, &AppController::onLoginAdd, this);
    _loginFrame->clear();
    _loginFrame->Show();
    _usersFrame->Hide();
}

void AppController::onUsersEdit(wxCommandEvent& e) {
    _loginFrame->Unbind(LOGIN_SUBMIT, &AppController::onLoginAdd, this);
    _loginFrame->Unbind(LOGIN_SUBMIT, &AppController::onLoginEdit, this);
    _loginFrame->Bind(LOGIN_SUBMIT, &AppController::onLoginEdit, this);
    _loginFrame->Show();
    _loginFrame->setInput(selectedUserData());
    _usersFrame->Hide();
}

void AppController::onUsersDelete(wxCommandEvent& e) {
    try {
        _usersManager->deleteUser(selectedUserData());
    }
    catch (Exception& e) {
        showException(e.what());
    }
    updateUserFrame();
}

void AppController::onLoginClose(wxCloseEvent& e) {
    _loginFrame->Hide();
    updateUserFrame();
}

void AppController::onLoginAdd(wxCommandEvent& e) {
    try {
        auto data = _loginFrame->userInput();
        _usersManager->addUser(data);
    }
    catch (Exception& e) {
        showException(e.what());
    }
    _loginFrame->Hide();
    updateUserFrame();
}

void AppController::onLoginEdit(wxCommandEvent& e) {
    try {
        auto data = _loginFrame->userInput();
        data.setDbId(selectedUserData().dbId());
        _usersManager->addUser(data, true);
    }
    catch (Exception& e) {
        showException(e.what());
    }
    
    _loginFrame->Hide();
    updateUserFrame();
}