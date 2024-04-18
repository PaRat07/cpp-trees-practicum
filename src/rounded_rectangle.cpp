#include "../drawers/rounded_rectangle.h"


RoundedRectangleShape::RoundedRectangleShape(sf::Vector2f size)
        : size_(size)
{
}

void RoundedRectangleShape::setSize(sf::Vector2f new_size) {
    size_ = new_size;
}

void RoundedRectangleShape::setPosition(sf::Vector2f new_pos) {
    pos_ = new_pos;
}

void RoundedRectangleShape::setFillColor(sf::Color new_color) {
    if (new_color == sf::Color::Transparent) throw std::runtime_error("RoundedRectangleShape doesn't support transparent fill color");
    fill_color_ = new_color;
}

void RoundedRectangleShape::setOutlineColor(sf::Color new_color) {
    outline_color_ = new_color;
}

void RoundedRectangleShape::setOutlineThickness(float thick) {
    outline_thickness_ = thick;
}

void RoundedRectangleShape::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    sf::CircleShape lu, ld, ru, rd;

    lu.setPointCount(50);
    ld.setPointCount(50);
    ru.setPointCount(50);
    rd.setPointCount(50);

    lu.setRadius(round_radius_);
    ld.setRadius(round_radius_);
    ru.setRadius(round_radius_);
    rd.setRadius(round_radius_);

    lu.setPosition(pos_.x + outline_thickness_, pos_.y + outline_thickness_);
    ld.setPosition(pos_.x + outline_thickness_, pos_.y + size_.y - round_radius_ * 2 - outline_thickness_);
    ru.setPosition(pos_.x + size_.x - round_radius_ * 2 - outline_thickness_, pos_.y + outline_thickness_);
    rd.setPosition(pos_.x + size_.x - round_radius_ * 2 - outline_thickness_, pos_.y + size_.y - round_radius_ * 2 - outline_thickness_);

    lu.setOutlineColor(outline_color_);
    ld.setOutlineColor(outline_color_);
    ru.setOutlineColor(outline_color_);
    rd.setOutlineColor(outline_color_);

    lu.setOutlineThickness(outline_thickness_);
    ld.setOutlineThickness(outline_thickness_);
    ru.setOutlineThickness(outline_thickness_);
    rd.setOutlineThickness(outline_thickness_);

    lu.setFillColor(fill_color_);
    ld.setFillColor(fill_color_);
    ru.setFillColor(fill_color_);
    rd.setFillColor(fill_color_);

    target.draw(lu);
    target.draw(ld);
    target.draw(ru);
    target.draw(rd);

    sf::RectangleShape horizontal(sf::Vector2f(size_.x - outline_thickness_ * 2, size_.y - round_radius_ * 2 - outline_thickness_ * 2));
    horizontal.setPosition(pos_.x + outline_thickness_, pos_.y + round_radius_ + outline_thickness_);
    horizontal.setFillColor(fill_color_);
    target.draw(horizontal);

    sf::RectangleShape vertical(sf::Vector2f(size_.x - round_radius_ * 2 - outline_thickness_ * 2, size_.y - outline_thickness_ * 2));
    vertical.setPosition(pos_.x + round_radius_ + outline_thickness_, pos_.y + outline_thickness_);
    vertical.setFillColor(fill_color_);
    target.draw(vertical);

    sf::RectangleShape u(sf::Vector2f(size_.x - round_radius_ * 2, outline_thickness_));
    u.setFillColor(outline_color_);
    u.setPosition(pos_.x + round_radius_, pos_.y);
    target.draw(u);

    sf::RectangleShape d(sf::Vector2f(size_.x - round_radius_ * 2, outline_thickness_));
    d.setFillColor(outline_color_);
    d.setPosition(pos_.x + round_radius_, pos_.y + size_.y - outline_thickness_);
    target.draw(d);

    sf::RectangleShape l(sf::Vector2f(outline_thickness_, size_.y - round_radius_ * 2));
    l.setFillColor(outline_color_);
    l.setPosition(pos_.x, pos_.y + round_radius_);
    target.draw(l);

    sf::RectangleShape r(sf::Vector2f(outline_thickness_, size_.y - round_radius_ * 2));
    r.setFillColor(outline_color_);
    r.setPosition(pos_.x + size_.x - outline_thickness_, pos_.y + round_radius_);
    target.draw(r);
}
