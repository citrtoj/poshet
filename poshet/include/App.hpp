#pragma once
#include <wx/wx.h>
#include <string>
#include <iostream>

#include "AppController.hpp"

class App : public wxApp {
protected:
    AppController* controller;
public:
    bool OnInit() {
        controller = new AppController(this);
        return true;
    }
    
    
    void OnUnhandledException(){
        try {
            throw;
        }
        catch (ServerException& e) {
            std::cout << "Server Exception: " << e.what() << "\nQuitting...\n";
            exit(EXIT_FAILURE);
        }
        catch (Exception& e) {
            std::cout << "Poshet Exception: " << e.what() << "\nQuitting...\n";
            exit(EXIT_FAILURE);
        }
        catch (std::exception& e) {
            std::cout << "C++ Exception: " << e.what() << "\nQuitting...\n";
            exit(EXIT_FAILURE);
        }
    }
};