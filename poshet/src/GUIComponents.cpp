#include "GUIComponents.hpp"

void GUIComponent::render(sf::RenderWindow& window) {
    this->draw(window);
    _updated = false;
}

void GUIComponent::update() {
    _updated = true;
    std::cout << "updated\n";
}

TextLabel::TextLabel(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, int fontSize, sf::Color textColor) :
    _size(size),
    _origin(origin),
    _bg(size),
    _text(text, font, fontSize),
    _bgColor(backgroundColor),
    _textColor(textColor)
{
    updateProps();
}

TextLabel::TextLabel (sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font) : 
    TextLabel(sf::Color::White, size, origin, text, font, GUI::DEFAULT_TEXT_SIZE, sf::Color::Black) {
    updateProps();
    std::cout << _origin.x;
}

void TextLabel::draw(sf::RenderWindow& window) {
    window.draw(_bg);
    window.draw(_text);
}

void TextLabel::updateProps() {
    _bg.setFillColor(_bgColor);
    _bg.setSize(_size);
    _bg.setPosition(_origin);
    _text.setFillColor(_textColor);
    _text.setPosition(_origin);

    update();
}

void TextLabel::updateText(const std::string& text) {
    _text.setString(text);
}

sf::FloatRect TextLabel::getGlobalBounds() {
    return _bg.getGlobalBounds();
}

void TextLabel::setBackgroundColor(const sf::Color color) {
    _bgColor = color;
    updateProps();
}

void TextLabel::setTextColor(const sf::Color color) {
    _textColor = color;
    updateProps();
}


TextButton::TextButton(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, int fontSize, sf::Color textColor) :
    TextLabel(backgroundColor, size, origin, text, font, fontSize, textColor) {}

TextButton::TextButton(sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font) :
    TextLabel(size, origin, text, font) {}


void TextButton::draw(sf::RenderWindow& window) {
    // add code for isHovered
    TextLabel::draw(window);
}

void ComponentContainer::handleEvent(const sf::Event& event) {
    for (auto* comp : components) {
        comp->handleEvent(event);
        if (comp->updated()) {
            _anyUpdated = true; //at least one element of the view needs to be redrawn
        }
    }
}

void ComponentContainer::render(sf::RenderWindow& window) {
    if (_anyUpdated) {
        for (auto* comp : components) {
            comp->render(window);
        }
        std::cout << "drew all again\n";
        _anyUpdated = false;
        window.display();
    }
}

void ComponentContainer::add(GUIComponent* comp) {
    components.push_back(comp);
}

Textbox::Textbox(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, int fontSize, sf::Color textColor) :
    TextLabel(backgroundColor, size, origin, text, font, fontSize, textColor),
    _buffer(text) {
        updateText(_buffer);
    }

Textbox::Textbox(sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font) :
    TextLabel(size, origin, text, font),
    _buffer(text) {
        updateText(_buffer);
    }

void Textbox::refreshText() {
    updateText(_buffer);
}

void Textbox::draw(sf::RenderWindow& window) {
    refreshText();
    TextLabel::draw(window);
}

std::string Textbox::buffer() {
    return _buffer;
}

void Textbox::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        update();
        sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
        if (_bg.getGlobalBounds().contains(mousePos)) {
            _isSelected = true;
            std::cout << "Textbox Selected!" << std::endl;
        } else {
            _isSelected = false;
        }
    } else if (event.type == sf::Event::TextEntered && _isSelected) {
        update();
        if (event.text.unicode == 8 && !_text.getString().isEmpty()) {
            _buffer.pop_back();
        } else /* if (event.text.unicode >= 32 && event.text.unicode < 128)*/ {
            
            char newChar = static_cast<char>(event.text.unicode);
            if (newChar == '\r') {
                _buffer += "\r\n";
            }
            else {_buffer += newChar;}
            std::cout << _buffer << "\n";
        }
    }
}