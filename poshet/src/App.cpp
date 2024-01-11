#include "App.hpp"

bool App::OnInit() {
    // set locale to UTF-8
    try {
		std::locale::global(std::locale(".UTF-8"));
	} catch (std::exception &) {
		std::setlocale(LC_ALL, ".UTF-8");
	}
    loginFrame = new LoginFrame("Edit user info");
    dashboardFrame = new DashboardFrame("Dashboard");
    usersFrame = new UsersFrame("All mailboxes");
    controller = new AppController(this, loginFrame, dashboardFrame, usersFrame);
    Bind(wxEVT_CLOSE_WINDOW, &App::OnClose, this);
    ::wxInitAllImageHandlers();
    return true;
}

int App::OnExit() {
    delete controller;
    loginFrame->Close();
    dashboardFrame->Close();
    usersFrame->Close();
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