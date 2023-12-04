#include "LoginView.hpp"

LoginView::LoginView(sf::RenderWindow& window) :
    _window(window),
    _background({window.getSize().x, window.getSize().y}, {0, 0}, sf::Color(255, 200, 145)),
    _loginText({300,  50}, {(window.getSize().x - 300) / 2, 0}, "Login to Poshet", _font),
    _submit({200, 100}, {(window.getSize().x - 200) / 2, 500}, "Login", _font)
{
    _font.loadFromFile("../thirdparty/SpaceMono/SpaceMono-Regular.ttf");
    _container << _background << _submit << _loginText;
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