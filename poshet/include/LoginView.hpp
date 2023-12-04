#pragma once
#include <SFML/Graphics.hpp>
#include "GUIComponents.hpp"

class LoginView {
private:
    sf::RenderWindow& _window;

    ComponentContainer _container;

    TextButton _submit;
    TextLabel _loginText;
    /*Textbox _fullname,
            _user,
            _pass,
            _smtpDomain,
            _pop3Domain
            ;*/
    sf::Font _font;

public:
    LoginView(sf::RenderWindow& window);

    void render();

    void handleEvent(const sf::Event& event);

    bool hasBeenSubmitted();
};