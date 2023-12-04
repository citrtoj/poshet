#include "LoginController.hpp"

LoginController::LoginController(sf::RenderWindow& window) :
    _window(window),
    _view(window)
{}

LoginModel LoginController::getUserInfo() {
    LoginModel model;
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
                        model._submitted = true;
                        break;
                    }
            }
            if (model._submitted) {
                break;
            }
        }
        _window.clear();
        _view.render();
        _window.display();
    }
    return model;
}