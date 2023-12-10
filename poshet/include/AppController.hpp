#pragma once

#include <string>
#include <wx/event.h>
#include <iostream>

#include "LoginFrame.hpp"
#include "DashboardFrame.hpp"
#include "Session.hpp"
#include "Exceptions.hpp"

class AppController {
protected:
    Session* session;
    LoginFrame* loginFrame;
    DashboardFrame* dashboardFrame;

    wxApp* mainApp;

    bool _isDashboardInit = false;
public:
    AppController(wxApp* app);

    void OnLogin(wxCommandEvent& e);
    void login();

    void OnClose(wxEvent& e);
};