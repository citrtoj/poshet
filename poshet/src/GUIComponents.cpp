#include "GUIComponents.hpp"

void GUIComponent::render(sf::RenderTarget& window, const sf::RenderStates &states) {
    this->draw(window, states);
    _updated = false;
}

void GUIComponent::update() {
    _updated = true;
}

Rectangle::Rectangle(sf::Vector2f size, sf::Vector2f origin, sf::Color color):
    _size(size), _origin(origin), _bgColor(color) {
    updateProps();
}

Rectangle::Rectangle(sf::Vector2f size, sf::Vector2f origin) :
    Rectangle(size, origin, sf::Color::White) {}

void Rectangle::draw(sf::RenderTarget& window, const sf::RenderStates &states) {
    window.draw(_bg, states);
}

sf::FloatRect Rectangle::getGlobalBounds() {
    return _bg.getGlobalBounds();
}

void Rectangle::setBackgroundColor(const sf::Color color) {
    _bgColor = color;
    updateProps();
}

void Rectangle::updateProps() {
    _bg.setFillColor(_bgColor);
    _bg.setPosition(_origin);
    _bg.setSize(_size);
    _updated = true;
}

TextLabel::TextLabel(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, int fontSize, sf::Color textColor) :
    Rectangle(size, origin, backgroundColor),
    _text(text, font, fontSize),
    _textColor(textColor)
{
    updateProps();
}

TextLabel::TextLabel (sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font) : 
    TextLabel(sf::Color::White, size, origin, text, font, GUI::DEFAULT_TEXT_SIZE, sf::Color::Black) {
    updateProps();
    std::cout << _origin.x;
}

void TextLabel::draw(sf::RenderTarget& window, const sf::RenderStates &states) {
    Rectangle::draw(window, states);
    window.draw(_text, states);
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

void TextLabel::setTextColor(const sf::Color color) {
    _textColor = color;
    updateProps();
}


TextButton::TextButton(sf::Color backgroundColor, sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font, int fontSize, sf::Color textColor) :
    TextLabel(backgroundColor, size, origin, text, font, fontSize, textColor) {}

TextButton::TextButton(sf::Vector2f size, sf::Vector2f origin, const char* text, sf::Font& font) :
    TextLabel(sf::Color::Blue, size, origin, text, font, GUI::DEFAULT_TEXT_SIZE, sf::Color::Black) {}


void TextButton::draw(sf::RenderTarget& window, const sf::RenderStates &states) {
    // add code for isHovered
    TextLabel::draw(window, states);
}

void TextButton::handleEvent(const sf::Event& event) {
    _isClicked = false;

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

        if (_bg.getGlobalBounds().contains(mousePos)) {
            _isClicked = true;
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

        _isHovered = _bg.getGlobalBounds().contains(mousePos);
    }
}

bool TextButton::isClicked() {
    return _isClicked; 
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

void Textbox::draw(sf::RenderTarget& window, const sf::RenderStates &states) {
    refreshText();
    TextLabel::draw(window, states);
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
            std::cout << "Textbox selected!" << std::endl;
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

void ComponentContainer::draw(sf::RenderTarget& window, const sf::RenderStates &states) {
    for (auto* comp : components) {
        comp->draw(window, states);
    }
}

void ComponentContainer::add(GUIComponent* comp) {
    components.push_back(comp);
}

ComponentContainer& ComponentContainer::operator<<(GUIComponent& comp) {
    add(&comp);
    return *this;
}

void ComponentContainer::handleEvent(const sf::Event& event) {
    for (auto* comp : components) {
        comp->handleEvent(event);
        if (comp->updated()) {
            update(); //at least one element of the view needs to be redrawn. we'll redraw everything
        }
    }
}