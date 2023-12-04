#include <iostream>
#include "GUIComponents.hpp"
#include "Utils.hpp"
#include "Config.hpp"
#include "Pop3Connection.hpp"
#include "LoginController.hpp"

#include <SFML/Graphics.hpp>


int main() {
    sf::RenderWindow window(sf::VideoMode(Config::getInstance().RES_WIDTH, Config::getInstance().RES_HEIGHT), Config::getInstance().APP_NAME);

    LoginController controller(window);

    auto x = controller.getUserInfo();

    auto y = controller.getUserInfo();
}