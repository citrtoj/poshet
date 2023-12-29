#include "AppController.hpp"

AppController::AppController(wxApp* app) :
    _mainApp(app),
    _loginFrame(new LoginFrame("Login to Poshet")),
    _dashboardFrame(new DashboardFrame("Poshet"))
{
    // todo: perhaps get config settings before this??
    try {
        _fileManager = new MailFileManager();
        _session = new Session(_fileManager);
    }
    catch (Exception& e) {
        showException(e.what());
    }
    _loginFrame->subscribe(this);
    _dashboardFrame->subscribe(this);

    _loginFrame->Show(true);
}

void AppController::showException(const std::string& msg) {
    wxMessageBox(msg, "Error", wxOK | wxICON_ERROR);
}

void AppController::warnUnimplemented() {
    std::cout << "Feature not yet implemented\n";
}

void AppController::onCloseApp() {
    delete _session;
    _mainApp->Exit();
}

void AppController::onLoginSubmit() {
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

void AppController::onRefreshMailList() {
    getSetMail(true);
}

void AppController::getSetMail(bool force) {
    try {
        const auto& mails = _session->retrieveMail(force);
        _dashboardFrame->setMailList(mails);
    }
    catch (Exception& e) {
        showException(e.what());
        onCloseApp();
    }
}

void AppController::onSelectMail() {
    auto selected = _dashboardFrame->selected();
    const auto& mail = _session->getMail(selected);
    _dashboardFrame->updateViewMailPanel(mail);
}

void AppController::onNewMail() {
    if (!_isMailCreatorOpen) {
        _mailCreatorFrame = new MailCreatorFrame();
        _mailCreatorFrame->subscribe(this);
        _mailCreatorFrame->Show();
        _isMailCreatorOpen = true;
    }
}

void AppController::onReplyMail() {
    warnUnimplemented();
}

void AppController::onForwardMail() {
    warnUnimplemented();
}

void AppController::onDeleteMail() {
    auto selected = _dashboardFrame->selected();
    _session->deleteMail(selected);
    getSetMail(true);
}

void AppController::onMailCreatorSend() {
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
    onRefreshMailList();
}

void AppController::onMailCreatorClose() {
    // todo: recheck this
    // shouldn't lead to bugs, despite it seemingly being a "i'm pretending it's done before it's actually done" situation
    // the GUI is singlethreaded, so immediately after this is set to false, the window of the mail creator frame receives the close event, and then that starts closing itself, more safely than I would have closed it manually using Destroy();
    // and only then would the user theoretically be even able to press the "new message" button again
    // perhaps in the future I might allow more than one window
    // but that would mean a more complicated subscriber system, I think
    // or the creator would have to send a pointer/index to itself... or something
    _isMailCreatorOpen = false;
}

void AppController::onAttachmentClick(int index) {
    // todo: do stuff. open a Save As window, use FileManager to save the file locally, etc etc
    std::cout << "att index: " << index << "\n";
}

AppController::~AppController() {
    // destroy UI
    _loginFrame->Destroy();
    _dashboardFrame->Destroy();
    _mailCreatorFrame->Destroy();

    // destroy business-logic
    delete _fileManager;
    delete _session;
}