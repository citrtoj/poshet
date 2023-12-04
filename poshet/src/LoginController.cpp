#include "LoginController.hpp"

LoginController::LoginController(sf::RenderWindow& window) :
    _window(window),
    _view(window)
{}

LoginModel LoginController::getUserInfo() {
    while (_window.isOpen()) {
        sf::Event tmpEvent;
        if(_window.pollEvent(tmpEvent)) {
            switch(tmpEvent.type) {
                case sf::Event::Closed:
                    _window.close();
                    break;
                default:
                    _view.handleEvent(tmpEvent);
                    if (_view.hasBeenSubmitted()) {
                        // TODO grab all info from view
                        _model._submitted = true;
                        break;
                    }
            }
        }
        _view.render();
    }
    return _model;
}