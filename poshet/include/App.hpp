#pragma once
#include <wx/wx.h>
#include <string>
#include <iostream>

#include "AppController.hpp"

class App : public wxApp {
protected:
    AppController* controller;
public:
    bool OnInit();
    
    void OnUnhandledException();
};