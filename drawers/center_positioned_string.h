#pragma once

#include "SFML/Graphics.hpp"

#include "../app/colors.h"
#include "../app/general_data.h"

class CenterPositionedString : public sf::Drawable {
 public:
    void setPosition(float x, float y);

    void setPosition(sf::Vector2f pos);

    void setString(sf::String s);

    void setBackground(sf::Color color);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setTextColor(sf::Color color) {
        text_color_ = color;
    }

    void SetFontSize(float font_size) {
        font_size_ = font_size;
    }
 private:
    sf::Color text_color_ = on_primary_container;
    sf::String data_;
    float font_size_ = 30;
    sf::Vector2f pos_ = { 0, 0 };
};
