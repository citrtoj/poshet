#pragma once

#include "LoginModel.hpp"
#include "LoginView.hpp"

class LoginController {
protected:
    LoginModel& _model;
    LoginView&  _view;
public:
    LoginController(LoginModel& model, LoginView& view);
};