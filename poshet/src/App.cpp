#include "App.hpp"

bool App::OnInit() {
    controller = new AppController(this);
    ::wxInitAllImageHandlers();
    return true;
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