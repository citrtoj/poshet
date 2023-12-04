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
    static const int DEFAULT_TEXT_SIZE = 24;
}

class GUIComponent {
protected:
    bool _updated = true;
public:
    GUIComponent() {}
    virtual void draw(sf::RenderTarget& window, const sf::RenderStates &states = sf::RenderStates::Default) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;

    void update();
    bool updated() { return _updated; }
    void render(sf::RenderTarget& window, const sf::RenderStates &states = sf::RenderStates::Default);
};

class Rectangle : public GUIComponent {
protected:
    sf::RectangleShape _bg;
    sf::Vector2f _size, _origin;
    sf::Color _bgColor;

    virtual void updateProps();
public:
    Rectangle(sf::Vector2f size, sf::Vector2f origin, sf::Color color);
    Rectangle(sf::Vector2f size, sf::Vector2f origin);

    void draw(sf::RenderTarget& window, const sf::RenderStates &states = sf::RenderStates::Default) override;
    void handleEvent(const sf::Event& event) override {}

    sf::FloatRect getGlobalBounds();
    void setBackgroundColor(const sf::Color);
};

class TextLabel : public Rectangle {
protected:
    sf::Text _text;
    sf::Color _textColor;

    void updateProps();
public:
    TextLabel(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, int fontSize, sf::Color textColor);

    TextLabel(sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font);

    void draw(sf::RenderTarget& window, const sf::RenderStates &states = sf::RenderStates::Default) override;
    void handleEvent(const sf::Event& event) {}

    void updateText(const std::string& text);
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

    void draw(sf::RenderTarget& window, const sf::RenderStates &states = sf::RenderStates::Default) override;
    void handleEvent(const sf::Event& event) override;
    bool isClicked();
};

class Textbox : public TextLabel {
protected:
    std::string _buffer;
    std::string _placeholder;
    bool _isSelected;

    // bool _isCursorVisible;
    // sf::Clock _cursorClock;

    void refreshText();
public:
    Textbox(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, int fontSize, sf::Color textColor);

    Textbox(sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font);

    void draw(sf::RenderTarget& window, const sf::RenderStates &states = sf::RenderStates::Default) override;
    void handleEvent(const sf::Event& event) override;

    std::string buffer();
};

class ComponentContainer : public GUIComponent {
protected:
    std::vector<GUIComponent*> components;
public:
    ComponentContainer() {};
    void draw(sf::RenderTarget& window, const sf::RenderStates &states = sf::RenderStates::Default) override;
    void add(GUIComponent* comp);
    void handleEvent(const sf::Event& event) override;
    ComponentContainer& operator<<(GUIComponent& comp);
};