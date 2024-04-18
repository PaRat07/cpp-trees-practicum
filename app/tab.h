#pragma once

#include <memory>
#include <vector>

#include "SFML/Graphics.hpp"


class AbstractDrawer : public sf::Drawable {
public:
    virtual void ProcessEvent(sf::Event event) = 0;
    virtual ~AbstractDrawer() = default;
};

class Tab : public sf::Drawable {
public:
    Tab(const Tab &other) = delete;
    Tab(Tab &&other) = default;
    Tab() = default;

    template<class Elem>
    void AddElement(std::unique_ptr<Elem> new_elem) {
        elements_.emplace_back(std::move(new_elem));
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void ProcessEvent(sf::Event event);

private:
    std::vector<std::unique_ptr<AbstractDrawer>> elements_;
};
