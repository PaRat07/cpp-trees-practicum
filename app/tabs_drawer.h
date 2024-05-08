#pragma once

#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <future>
#include <optional>
#include <cmath>

#include "tab.h"
#include "general_data.h"
#include "colors.h"



class TabsManager : public AbstractDrawer {
    struct Animation {
        size_t from, to;
        std::chrono::time_point<std::chrono::steady_clock> beg, end;
    };

public:
    TabsManager(sf::Vector2f pos, sf::Vector2f size);

    void AddTab(Tab &&tab);

    void ProcessEvent(sf::Event event) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
private:
    size_t active_ = 0;
    mutable std::optional<Animation> anim_;
    std::vector<Tab> tab_;
    sf::Vector2f pos_, size_;

    static double GetPercantage(double time_gone);
};
