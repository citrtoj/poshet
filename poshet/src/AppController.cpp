#include "AppController.hpp"

AppController::AppController(wxApp* app) :
    _mainApp(app),
    _session(new Session()),
    _loginFrame(new LoginFrame("Login to Poshet")),
    _dashboardFrame(new DashboardFrame("Poshet"))
{
    _loginFrame->subscribe(this);
    _dashboardFrame->subscribe(this);

    _loginFrame->Show(true);
}

void AppController::login() {
    try {
        _session->setLoginData(_loginFrame->userInput());
        _session->connectAndLoginToServers();
        _loginFrame->Hide();
        _dashboardFrame->Show();
        this->onRefreshMailList();
    }
    catch (Exception& e) {
        _loginFrame->showError(e.what());
    }
}

void AppController::onCloseAnyWindow() {
    delete _session;
    _mainApp->Exit();
}

void AppController::onNewMail() {
    if (!_isMailCreatorOpen) {
        _mailCreatorFrame = new MailCreatorFrame();
        _mailCreatorFrame->subscribe(this);
        _mailCreatorFrame->Show();
        _isMailCreatorOpen = true;
    }
}

void AppController::onMailCreatorSend() {
    auto to = _mailCreatorFrame->to();
    auto subject = _mailCreatorFrame->subject();
    auto body = _mailCreatorFrame->body();

    _session->sendMail(to, subject, body);
    
}

void AppController::onMailCreatorClose() {
    //shouldn't lead to bugs, despite it seemingly being a "i'm pretending it's done before it's actually done" situation
    // the GUI is singlethreaded, so immediately after this is set to false, the window of the mail creator frame receives the close event, and then that starts closing itself, more safely than I would have closed it manually using Destroy();
    // and only then would the user theoretically be even able to press the "new message" button again
    // perhaps in the future I might allow more than one window
    // but that would mean a more complicated subscriber system, I think
    // or the creator would have to send a pointer/index to itself... or something
    _isMailCreatorOpen = false;
}