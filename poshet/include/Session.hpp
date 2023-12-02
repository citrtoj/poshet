#pragma once

#include "Pop3Connection.hpp"
#include "DatabaseConnection.hpp"
#include "DashboardView.hpp"
#include "LoginModel.hpp"

class Session {
protected:
    DashboardView _view;
    Pop3Connection _pop3;
    DatabaseConnection _db;
public:
    Session() : _db(Utils::DB_PATH) {}
    void authenticateUser(const LoginModel& model);
};