#include "LoginView.hpp"

LoginView::LoginView(sf::RenderWindow& window) :
    _window(window),
    _submit({200, 100}, {0, 0}, "Login", _font),
    _loginText({300,  100}, {300, 100}, "Login", _font)
{
    _font.loadFromFile("../utils/fonts/montserrat/static/Montserrat-Bold.ttf");
    _container.add(&_submit);
    _container.add(&_loginText);
}

void LoginView::render() {
    _container.render(_window);
}

void LoginView::handleEvent(const sf::Event& event) {
    _container.handleEvent(event);
}

bool LoginView::hasBeenSubmitted() {
    return _submit.isClicked();
}