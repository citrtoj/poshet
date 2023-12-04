#pragma once

#include "LoginModel.hpp"
#include "LoginView.hpp"

class LoginController {
protected:
    sf::RenderWindow& _window;
    LoginModel _model;
    LoginView  _view;
public:
    LoginController(sf::RenderWindow& window);

    LoginModel getUserInfo();
};