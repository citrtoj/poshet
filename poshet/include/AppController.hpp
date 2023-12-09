#pragma once
#include "LoginFrame.hpp"
#include "DashboardFrame.hpp"
#include "Session.hpp"
#include "Exceptions.hpp"
#include <string>
#include <iostream>

class AppController : wxEvtHandler {
protected:
    Session* session;
    LoginFrame* loginFrame;
    DashboardFrame* dashboardFrame;

    wxApp* mainApp;

    bool _isDashboardInit = false;
public:
    AppController(wxApp* app);

    void OnLogin(wxCommandEvent& e);
    void OnClose(wxEvent& e);
};