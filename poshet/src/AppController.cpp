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

    // _loginFrame->loginButton()->Bind(wxEVT_BUTTON, &AppController::OnLogin, this);
    
    
    
    // _loginFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::OnClose, this);
    // _dashboardFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::OnClose, this);
}

void AppController::login() {
    try {
        LoginData data(_loginFrame->userInput());
        _session->setLoginData(data);
        _session->login();
        _loginFrame->Hide();
        _dashboardFrame->Show();
        _session->retrieveMail();
    }
    catch (Exception& e) {
        _loginFrame->showError(e.what());
    }
}

void AppController::onCloseAnyWindow() {
    std::cout << "here\n";
    delete _session;
    std::cout << "here\n";
    _mainApp->Exit();
}