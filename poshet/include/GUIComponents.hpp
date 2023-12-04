#pragma once
#include <SFML/Graphics.hpp>

#include <iostream>

namespace GUI {
    enum Align {
        TOP_LEFT,
        TOP_CENTER,
        CENTER_LEFT,
        CENTER_CENTER
    };
    static const int DEFAULT_TEXT_SIZE = 12;
}

class GUIComponent {
protected:
    bool _updated = true;
public:
    GUIComponent() {}
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;
    void update();
    bool updated() { return _updated; }
    void render(sf::RenderWindow& window);
};

class TextLabel : public GUIComponent {
protected:
    sf::RectangleShape _bg;
    sf::Text _text;
    sf::Color _bgColor, _textColor;

    sf::Vector2f _size, _origin;

    void updateProps();
public:
    TextLabel(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, int fontSize, sf::Color textColor);

    TextLabel(sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font);

    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event) {}

    sf::FloatRect getGlobalBounds();

    void updateText(const std::string& text);
    void setBackgroundColor(const sf::Color);
    void setTextColor(const sf::Color);
};

class TextButton : public TextLabel {
protected:
    bool _isHovered = false;
    bool _isClicked = false;

    sf::Color _hoverBg;
public:
    TextButton(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, int fontSize, sf::Color textColor);

    TextButton(sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font);

    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event) {
        _isClicked = false;

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

            if (_bg.getGlobalBounds().contains(mousePos)) {
                _isClicked = true;
                std::cout << "clicked\n";
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

            _isHovered = _bg.getGlobalBounds().contains(mousePos);
        }
    }
    bool isClicked() { return _isClicked; }
};

class Textbox : public TextLabel {
protected:
    std::string _buffer;
    bool _isSelected;

    bool _isCursorVisible;
    sf::Clock _cursorClock;

    void refreshText();
public:
    Textbox(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, int fontSize, sf::Color textColor);

    Textbox(sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font);

    void draw(sf::RenderWindow&);
    void handleEvent(const sf::Event& event);

    std::string buffer();
};

class ComponentContainer {
protected:
    std::vector<GUIComponent*> components;
    bool _anyUpdated;
public:
    ComponentContainer() {};
    void render(sf::RenderWindow& window);
    void add(GUIComponent* comp);

    void handleEvent(const sf::Event& event);
};