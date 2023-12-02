#pragma once
#include "LoginView.hpp"
#include "LoginModel.hpp"


class LoginController {
private:
    LoginModel* _loginModel;
    LoginView* _loginView;
public:
    LoginController(LoginModel* model, LoginView* view);
};