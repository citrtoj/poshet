#include "GUIComponents.hpp"

void GUIComponent::render(sf::RenderWindow& window) {
    if (_updated) {
        this->draw(window);
        _updated = false;
    }
}

void GUIComponent::update() {
    _updated = true;
}

RectangleComponent::RectangleComponent(sf::Vector2f size, sf::Vector2f origin) : _size(size), _origin(origin) {}


void Clickable::isMouseOver(sf::RenderWindow& window) {
    
}

TextLabel::TextLabel(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, unsigned int fontSize, sf::Color textColor, sf::Vector2f coords, GUI::Align alignment) :
    RectangleComponent(size, origin),
    _bg(size),
    _text(text, font, fontSize)
{
    _bg.setFillColor(backgroundColor);
    _text.setFillColor(backgroundColor);
}

void TextLabel::draw(sf::RenderWindow& window) {
    window.draw(_bg);
    window.draw(_text);
}


TextButton::TextButton(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, char* text, sf::Font& font, unsigned int fontSize, sf::Color textColor, sf::Vector2f coords, GUI::Align alignment) :
    Clickable(size, origin),
    _label(backgroundColor, size, origin, text, font, fontSize, textColor, coords, alignment),
    _isHovered(false) {}

void TextButton::draw(sf::RenderWindow& window) {
    _label.draw(window);
}


void ComponentCollection::draw(sf::RenderWindow& window) {
    for (auto* comp : components) {
        comp->draw(window);
    }
}

void ComponentCollection::add(GUIComponent* comp) {
    components.push_back(comp);
}