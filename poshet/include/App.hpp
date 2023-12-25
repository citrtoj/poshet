#pragma once
#include <string>
#include <iostream>

#include <wx/wx.h>

#include "AppController.hpp"

class App : public wxApp {
protected:
    AppController* controller;
public:
    bool OnInit();
    
    void OnUnhandledException();
};