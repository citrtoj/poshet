#pragma once
#include <SFML/Graphics.hpp>
#include "LoginView.hpp"
#include "LoginController.hpp"
#include "LoginModel.hpp"

class App {
private:
    sf::RenderWindow _window;
    LoginModel _loginModel;
    LoginView _loginView;
    LoginController _loginController;//(&_loginModel, &_loginView);
public:
    App();
};