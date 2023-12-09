#pragma once
#include <wx/wx.h>
#include <string>
#include "AppController.hpp"
#include <iostream>

class App : public wxApp {
protected:
    AppController* controller;
public:
    bool OnInit() {
        controller = new AppController(this);
        return true;
    }

    void OnLogin(wxCommandEvent& e) {
        std::cout << "logging\n";
    }

    void OnCloseLoginWindow(wxEvent& e) {
        Exit();
    }
    
    void OnUnhandledException(){
        try {
            throw;
        }
        catch (Exception& e) {
            std::cout << "Exception: " << e.what() << "\nQuitting...\n";
            exit(EXIT_FAILURE);
        }
    }
};