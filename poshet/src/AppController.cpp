#include "AppController.hpp"

AppController::AppController(wxApp* app) :
    mainApp(app),
    session(new Session()),
    loginFrame(new LoginFrame("Login to Poshet")), dashboardFrame(new DashboardFrame("Poshet"))
{
    loginFrame->Show(true);
    loginFrame->loginButton()->Bind(wxEVT_BUTTON, &AppController::OnLogin, this);
    loginFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::OnClose, this);
    dashboardFrame->Bind(wxEVT_CLOSE_WINDOW, &AppController::OnClose, this);
}

void AppController::OnLogin(wxCommandEvent& e) {
    try {
        LoginData data(loginFrame->userInput());
        session->setLoginData(data);
        session->login();
        loginFrame->Hide();
        dashboardFrame->Show();
    }
    catch (Exception& e) {
        wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR);
    }
}

void AppController::OnClose(wxEvent& e) {
    delete session;
    mainApp->Exit();
}