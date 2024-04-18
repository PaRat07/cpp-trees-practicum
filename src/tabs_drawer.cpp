#include "../app/tabs_drawer.h"
#include "../drawers/rounded_rectangle.h"

double TabsManager::GetPercantage(double time_gone) {
    return std::sin(time_gone * M_PI_2);
}

void TabsManager::ProcessEvent(sf::Event event) {
    switch (event.type) {
        case sf::Event::TextEntered: {
            // ctrl + tab
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) &&
                event.text.unicode == 9) {
                if (tab_.size() <= 1) return;
                anim_ = Animation   {
                        .from = active_,
                        .to = active_ = (active_ + 1) % tab_.size(),
                        .beg = std::chrono::steady_clock::now(),
                        .end = std::chrono::steady_clock::now() + std::chrono::milliseconds(350)
                };
                break;
            }
        }
        case sf::Event::MouseButtonPressed:
            event.mouseButton.x -= pos_.x * win_size.x;
            event.mouseButton.y -= pos_.y * win_size.y;
            tab_[active_].ProcessEvent(event);
            break;
        case sf::Event::MouseMoved:
            event.mouseMove.x -= pos_.x * win_size.x;
            event.mouseMove.y -= pos_.y * win_size.y;
            tab_[active_].ProcessEvent(event);
            break;
        default: {
            tab_[active_].ProcessEvent(event);
            break;
        }
    }

}

void TabsManager::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    sf::Vector2f real_size(size_.x * win_size.x, size_.y * win_size.y);
    sf::Vector2f real_pos(pos_.x * win_size.x, pos_.y * win_size.y);
    sf::RenderTexture texture;
    texture.create(real_size.x, real_size.y);
    texture.clear(surface);

    if (anim_) {
        auto cur_time = std::chrono::steady_clock::now();
        if (anim_.has_value() && cur_time > anim_->end) {
            anim_.reset();
            texture.draw(tab_[active_]);
        } else {
            double percentage = 1. - GetPercantage(1. * (cur_time - anim_->beg).count() / (anim_->end - anim_->beg).count());
            int border = real_size.x * percentage;
            {
                sf::RenderTexture from;
                from.create(real_size.x, real_size.y);
                from.clear(surface);
                {
                    RoundedRectangleShape frame(real_size);
                    frame.setRoundRadius(10);
                    frame.setFillColor(surface_container);
                    from.draw(frame);
                }
                from.draw(tab_[anim_->from]);
                from.display();
                sf::Sprite sprite;
                sprite.setTexture(from.getTexture());
                sprite.setPosition(border - real_size.x - 5, 0);
                texture.draw(sprite);
            }

            {
                sf::RenderTexture to;
                to.create(real_size.x, real_size.y);
                to.clear(surface);
                {
                    RoundedRectangleShape frame(real_size);
                    frame.setRoundRadius(10);
                    frame.setFillColor(surface_container);
                    to.draw(frame);
                }
                to.draw(tab_[anim_->to]);
                to.display();
                sf::Sprite sprite;
                sprite.setTexture(to.getTexture());
                sprite.setPosition(border, 0);
                texture.draw(sprite);
            }
        }
    } else {
        {
            RoundedRectangleShape frame(real_size);
            frame.setRoundRadius(10);
            frame.setFillColor(surface_container);
            texture.draw(frame);
        }
        texture.draw(tab_[active_]);
    }

    texture.display();
    sf::Sprite sprite(texture.getTexture());
    sprite.setPosition(real_pos);
    target.draw(sprite);
}

void TabsManager::AddTab(Tab &&tab) {
    tab_.push_back(std::move(tab));
}

TabsManager::TabsManager(sf::Vector2f pos, sf::Vector2f size)
        : pos_(pos.x / win_size.x, pos.y / win_size.y)
        , size_(size.x / win_size.x, size.y / win_size.y)
{}
