#pragma once
#include <string>
#include <iostream>

#include <wx/wx.h>

#include "AppController.hpp"
#include "LoginFrame.hpp"
#include "DashboardFrame.hpp"

class App : public wxApp {
protected:
    AppController* controller;
    LoginFrame* loginFrame;
    DashboardFrame* dashboardFrame;
public:
    bool OnInit();
    int OnExit();
    void OnClose(wxCloseEvent& e);
    void OnUnhandledException();
};