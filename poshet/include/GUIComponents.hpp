#pragma once
#include <SFML/Graphics.hpp>

namespace GUI {
    enum Align {
        TOP_LEFT,
        TOP_CENTER,
        CENTER_LEFT,
        CENTER_CENTER
    };
}

class GUIComponent {
protected:
    bool _updated = false;
    
public:
    GUIComponent() {}
    virtual void draw(sf::RenderWindow& window) = 0;
    void update();
    void render(sf::RenderWindow& window);
};

class RectangleComponent : GUIComponent {
protected:
    sf::Vector2f _size;
    sf::Vector2f _origin;
public:
    RectangleComponent(sf::Vector2f size, sf::Vector2f origin);
    virtual void draw(sf::RenderWindow& window) = 0;
};

class Clickable : RectangleComponent {
public:
    virtual void draw(sf::RenderWindow& window) = 0;
    Clickable(sf::Vector2f size, sf::Vector2f origin) : RectangleComponent(size, origin) {}
    void isMouseOver(sf::RenderWindow& window);
};

class TextLabel : RectangleComponent {
protected:
    sf::RectangleShape _bg;
    sf::Text _text;
public:
    TextLabel(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, unsigned int fontSize, sf::Color textColor,
    sf::Vector2f coords, GUI::Align alignment);
    void draw(sf::RenderWindow& window);
};

class TextButton : Clickable {
protected:
    TextLabel _label;
    bool _isHovered;
public:
    TextButton(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, char* text, sf::Font& font, unsigned int fontSize, sf::Color textColor, sf::Vector2f coords, GUI::Align alignment);
    void draw(sf::RenderWindow& window);
};

class ComponentCollection {
protected:
    std::vector<GUIComponent*> components;
public:
    ComponentCollection() {};
    void draw(sf::RenderWindow& window);
    void add(GUIComponent* comp);
};