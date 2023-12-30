#include "App.hpp"

bool App::OnInit() {
    controller = new AppController(this);
    Bind(wxEVT_CLOSE_WINDOW, &App::OnClose, this);
    ::wxInitAllImageHandlers();
    return true;
}

int App::OnExit() {
    delete controller;
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
        throw;
    }
    catch (Exception& e) {
        std::cout << "Poshet Exception: " << e.what() << "\nQuitting...\n";
        throw;
    }
    catch (std::exception& e) {
        std::cout << "C++ Exception: " << e.what() << "\nQuitting...\n";
        throw;
    }
}