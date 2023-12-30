#include "App.hpp"

bool App::OnInit() {
    loginFrame = new LoginFrame("Login");
    dashboardFrame = new DashboardFrame("Dashboard");
    controller = new AppController(this, loginFrame, dashboardFrame);
    Bind(wxEVT_CLOSE_WINDOW, &App::OnClose, this);
    ::wxInitAllImageHandlers();
    return true;
}

int App::OnExit() {
    delete controller;
    loginFrame->Destroy();
    dashboardFrame->Destroy();
    return 0;
}

void App::OnClose(wxCloseEvent& e) {
    Exit();
}

void App::OnUnhandledException() {
    try {
        throw;
    }
    catch (ServerException& e) {
        std::cout << "Server Exception: " << e.what() << "\nQuitting...\n";
        Exit();
    }
    catch (Exception& e) {
        std::cout << "Poshet Exception: " << e.what() << "\nQuitting...\n";
        Exit();
    }
    catch (std::exception& e) {
        std::cout << "C++ Exception: " << e.what() << "\nQuitting...\n";
        Exit();
    }
}