#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

#include <functional>
#include <cassert>

#include "rounded_rectangle.h"
#include "center_positioned_string.h"
#include "../app/general_data.h"
#include "../app/tab.h"

class ButtonWithImage : public AbstractDrawer {
public:
    ButtonWithImage(sf::Vector2f pos, sf::Vector2f sz, std::string path, const std::function<void()> &cb);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void ProcessEvent(sf::Event event) override;

private:
    std::function<void()> callback_;
    std::string path_to_png_;
    sf::Vector2f size_, pos_;
};

class ButtonWithTextRelativePos : public AbstractDrawer {
public:
    ButtonWithTextRelativePos(sf::Vector2f pos, sf::Vector2f sz, std::string text, const std::function<void()> &cb);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void ProcessEvent(sf::Event event) override;

private:
    std::function<void()> callback_;
    std::string text_;
    sf::Vector2f size_, pos_;
};

class ButtonWithTextAbsPos : public AbstractDrawer {
public:
    ButtonWithTextAbsPos(sf::Vector2f pos, sf::Vector2f sz, std::string text, std::function<void()> cb);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void ProcessEvent(sf::Event event) override;


    void SetPosition(sf::Vector2f pos) {
        pos_ = pos;
    }

    sf::Vector2f GetPosition() const {
        return pos_;
    }

private:
    std::function<void()> callback_;
    std::string text_;
    sf::Vector2f size_, pos_;
};
