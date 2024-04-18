#include "../drawers/center_positioned_string.h"

void CenterPositionedString::setPosition(float x, float y) {
    pos_ = { x, y };
}

void CenterPositionedString::setPosition(sf::Vector2f pos) {
    pos_ = pos;
}

void CenterPositionedString::setString(sf::String s) {
    data_ = std::move(s);
}

void CenterPositionedString::setBackground(sf::Color color) {
    text_color_ = color;
}

void CenterPositionedString::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    sf::Text val;
    val.setFont(font);
    val.setFillColor(text_color_);
    val.setCharacterSize(letter_size);
    val.setString(data_);
    val.setPosition(pos_.x - val.getLocalBounds().width / 2, pos_.y - letter_size * 0.7);
    target.draw(val);
}
