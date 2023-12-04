#include "App.hpp"
#include "Config.hpp"

App::App() : _window(sf::VideoMode(Config::getInstance().RES_HEIGHT, Config::getInstance().RES_WIDTH), Config::getInstance().APP_NAME) {}
