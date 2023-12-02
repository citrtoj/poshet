#include "App.hpp"

App::App(): _window(sf::VideoMode(800, 600), Info::APP_NAME),
            _loginModel(),
            _loginView(&_window),
            _loginController(&_loginModel, &_loginView)
            {}