#pragma once

#include "SFML/Graphics.hpp"
#include "../core/physics/vector_operations.h"

class LineShape : public sf::Drawable {
public:
    LineShape(sf::Vector2f from, sf::Vector2f to)
        : from_(from)
        , to_(to)
    {}

    void setFillColor(sf::Color color) {
        color_ = color;
    }

    void setThickness(float thick) {
        thickness = thick;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        sf::Vector2f v(to_ - from_);
        sf::Vector2f norm(v.y, - v.x);
        norm *= thickness / 2 / CalcLength(norm);
        sf::Vector2f lu(from_ - norm), ld(from_ + norm), ru(to_ - norm), rd(to_ + norm);
        sf::ConvexShape convex;
        convex.setPointCount(4);
        convex.setPoint(0, ld);
        convex.setPoint(1, lu);
        convex.setPoint(2, ru);
        convex.setPoint(3, rd);
        convex.setFillColor(color_);
        target.draw(convex);
    }

private:
    sf::Vector2f from_, to_;
    float thickness = 1;
    sf::Color color_;
};