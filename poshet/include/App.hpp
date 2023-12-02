#pragma once
#include <SFML/Graphics.hpp>
#include "LoginView.hpp"
#include "LoginModel.hpp"

class App {
private:
    sf::RenderWindow _window;
    LoginModel model;
    LoginView view;
public:
    App();
};