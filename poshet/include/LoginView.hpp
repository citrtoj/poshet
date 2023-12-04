#pragma once
#include <SFML/Graphics.hpp>
#include "GUIComponents.hpp"
#include <unordered_map>

class LoginView {
private:
    sf::RenderWindow& _window;

    ComponentContainer _container;

    Rectangle _background;
    TextButton _submit;
    TextLabel _loginText;
    std::unordered_map<std::string, Textbox*> _textboxes;
    sf::Font _font;

public:
    LoginView(sf::RenderWindow& window);
    void render();
    void handleEvent(const sf::Event& event);
    bool hasBeenSubmitted();
};