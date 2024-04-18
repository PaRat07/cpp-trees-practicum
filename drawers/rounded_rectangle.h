#pragma once

#include "SFML/Graphics.hpp"
#include <iostream>

class RoundedRectangleShape : public sf::Drawable {
 public:
    explicit RoundedRectangleShape(sf::Vector2f size);

    void setSize(sf::Vector2f new_size);

    const sf::Vector2f &getSize() const {
        return size_;
    }

    void setPosition(sf::Vector2f new_pos);

    const sf::Vector2f &getPosition() const {
        return pos_;
    }

    void setFillColor(sf::Color new_color);

    void setOutlineColor(sf::Color new_color);

    const sf::Color &getFillColor() const {
        return fill_color_;
    }

    const sf::Color &getOulineColor() const {
        return outline_color_;
    }

    void setOutlineThickness(float thick);

    float getOutlineThickness() const {
        return outline_thickness_;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
    void setRoundRadius(float r) {
        round_radius_ = r;
    }

 private:
    float outline_thickness_ = 1;
    sf::Vector2f pos_{ 0.f, 0.f}, size_;
    sf::Color outline_color_ = sf::Color::Black, fill_color_ = sf::Color::Transparent;
    float round_radius_ = 0.;
};
