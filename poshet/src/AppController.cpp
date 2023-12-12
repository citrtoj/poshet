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
        LoginData data(_loginFrame->userInput());
        _session->setLoginData(data);
        _session->login();
        _loginFrame->Hide();
        _dashboardFrame->Show();
    }
    catch (Exception& e) {
        _loginFrame->showError(e.what());
    }
}

void AppController::onCloseAnyWindow() {
    delete _session;
    _mainApp->Exit();
}