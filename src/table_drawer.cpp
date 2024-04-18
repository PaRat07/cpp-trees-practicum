#include "../drawers/table_drawer.h"

sf::Vector2i TableDrawer::cell_size = { 5000, 40 };

TableDrawer::TableDrawer(sf::Vector2f pos,
                         sf::Vector2f size,
                         Table &machine,
                         bool allow_change_text,
                         std::unique_ptr<HeaderDrawer> hor_header,
                         std::unique_ptr<HeaderDrawer> ver_header)
        : machine_(machine)
        , pos_(pos.x / win_size.x, pos.y / win_size.y)
        , size_(size.x / win_size.x, size.y / win_size.y)
        , can_change_text_(allow_change_text)
        , hor_header_(std::move(hor_header))
        , ver_header_(std::move(ver_header))
{}

void TableDrawer::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    sf::Vector2i real_size(size_.x * win_size.x, size_.y * win_size.y);
    sf::Vector2f real_pos(pos_.x * win_size.x, pos_.y * win_size.y);

    {
        RoundedRectangleShape frame{sf::Vector2f(real_size.x - LINE_THICKNESS * 2, real_size.y - LINE_THICKNESS * 2)};
        frame.setRoundRadius(10);
        frame.setPosition(sf::Vector2f(real_pos.x + LINE_THICKNESS, real_pos.y + LINE_THICKNESS));
        frame.setFillColor(surface_container);
        frame.setOutlineThickness(0);
        target.draw(frame);
    }

    // net + vals
    {
        sf::Sprite net_sprite;
        sf::RenderTexture net_texture;
        net_texture.create(real_size.x - ver_header_->Size() - PADDING * 2, real_size.y - hor_header_->Size() - PADDING * 2);
        net_texture.clear(surface_container);
        sf::RectangleShape line(sf::Vector2f(real_size.x - PADDING * 2, LINE_THICKNESS));
        line.setFillColor(outline);
        for (int i = pos_in_.y % cell_size.y; i < std::min(real_size.y, machine_.Size().y * cell_size.y); i += cell_size.y) {
            line.setPosition(0, i);
            net_texture.draw(line);
        }

        line.setSize(sf::Vector2f(LINE_THICKNESS, std::min(real_size.y - PADDING * 2, machine_.Size().y * cell_size.y)));
        for (int i = pos_in_.x % cell_size.x; i <= std::min(real_size.x, machine_.Size().x * cell_size.x); i += cell_size.x) {
            line.setPosition(i, 0);
            net_texture.draw(line);
        }

        {
            sf::Text str;
            str.setFillColor(on_surface_variant);
            str.setFont(font);
            str.setCharacterSize(letter_size);
            for (int i = -pos_in_.x / cell_size.x; i * cell_size.x + pos_in_.x < real_size.x && i < machine_.Size().x; ++i) {
                for (int j = -pos_in_.y / cell_size.y; j * cell_size.y + pos_in_.y < real_size.y && j < machine_.Size().y; ++j) {
                    str.setString(machine_.GetValue(sf::Vector2i(i, j)));
                    str.setPosition(i * cell_size.x + LINE_THICKNESS + 5 + pos_in_.x,
                                    j * cell_size.y + cell_size.y / 2 + pos_in_.y - letter_size * 0.7);
                    net_texture.draw(str);
                }
            }
        }
        if (field.has_value()) {
            net_texture.draw(*field);
        }
        net_texture.display();
        net_sprite.setTexture(net_texture.getTexture());
        net_sprite.setPosition(ver_header_->Size() + PADDING + real_pos.x, PADDING + real_pos.y + hor_header_->Size());
        target.draw(net_sprite);
    }

    // hor
    if (hor_header_->Size() > 0){
        sf::Sprite sprite;
        sf::RenderTexture texture;
        texture.create(real_size.x - ver_header_->Size() - LINE_THICKNESS, hor_header_->Size());
        texture.clear(surface_container);
        texture.draw(*hor_header_);
        texture.display();
        sprite.setTexture(texture.getTexture());
        sprite.setPosition(real_pos.x + ver_header_->Size() + LINE_THICKNESS, real_pos.y + LINE_THICKNESS);
        target.draw(sprite);
    }

    // ver
    if (ver_header_->Size() > 0){
        sf::Sprite sprite;
        sf::RenderTexture texture;
        texture.create(ver_header_->Size(), real_size.y - PADDING * 2 - hor_header_->Size());
        texture.clear(surface_container);
        texture.draw(*ver_header_);
        texture.display();
        sprite.setTexture(texture.getTexture());
        sprite.setPosition(real_pos.x + PADDING, real_pos.y + hor_header_->Size() + PADDING);
        target.draw(sprite);
    }

    // hor line(not texture)
    {
        sf::RectangleShape legend_border(sf::Vector2f(real_size.x - PADDING * 2, LINE_THICKNESS));
        legend_border.setFillColor(outline);
        legend_border.setPosition(sf::Vector2f(real_pos.x + PADDING, real_pos.y + PADDING + hor_header_->Size()));
        target.draw(legend_border);
    }

    // ver line(not texture)
    {
        sf::RectangleShape legend_border(sf::Vector2f(LINE_THICKNESS, std::min(real_size.y - PADDING * 2 - (int)hor_header_->Size() - LINE_THICKNESS, machine_.Size().y * cell_size.y + (int)hor_header_->Size())));
        legend_border.setFillColor(outline);
        legend_border.setPosition(sf::Vector2f(real_pos.x + PADDING + ver_header_->Size(), real_pos.y + PADDING + hor_header_->Size() - LINE_THICKNESS));
        target.draw(legend_border);
    }

    // frame
    {
        sf::RectangleShape table_border(sf::Vector2f( (float)std::min(real_size.x - PADDING * 2, machine_.Size().x * cell_size.x + (int)ver_header_->Size()),
                                                           (float)std::min(real_size.y - PADDING * 2, machine_.Size().y * cell_size.y + (int)hor_header_->Size())));
        table_border.setPosition(sf::Vector2f(real_pos.x + PADDING, real_pos.y + PADDING));
        table_border.setFillColor(sf::Color::Transparent);
        table_border.setOutlineThickness(LINE_THICKNESS);
        table_border.setOutlineColor(outline);
        target.draw(table_border);
    }
}

void TableDrawer::ProcessEvent(sf::Event event) {
    switch (event.type) {
        case sf::Event::EventType::MouseWheelScrolled: {
            if (!hovered_) break;
            if (event.mouseWheelScroll.wheel == sf::Mouse::Wheel::VerticalWheel) {
                pos_in_.y += event.mouseWheelScroll.delta * 4;
                pos_in_.y = std::max(pos_in_.y, (int) (-GetTableSize().y + size_.y * win_size.y - PADDING * 2));
                pos_in_.y = std::min(pos_in_.y, 0);
            } else {
                pos_in_.x += event.mouseWheelScroll.delta * 4;
                pos_in_.x = std::max(pos_in_.x, (int) (-GetTableSize().x + size_.x * win_size.x - PADDING * 2));
                pos_in_.x = std::min(pos_in_.x, 0);
            }
            ver_header_->SetShift(pos_in_.y);
            hor_header_->SetShift(pos_in_.x);
            if (field.has_value()) {
                field->SetPosition(sf::Vector2f(active_pos_->x * cell_size.x + pos_in_.x + 2,
                                                active_pos_->y * cell_size.y + pos_in_.y + 2));
            }
            break;
        }
        case sf::Event::EventType::MouseButtonPressed: {
            {
                sf::Event ev_for_ver_header = event;
                ev_for_ver_header.mouseButton.y -= PADDING;
                ev_for_ver_header.mouseButton.x -= PADDING + hor_header_->Size();
                ver_header_->ProcessEvent(ev_for_ver_header);
            }
            {
                sf::Event ev_for_hor_header = event;
                ev_for_hor_header.mouseButton.y -= PADDING + ver_header_->Size();
                ev_for_hor_header.mouseButton.x -= PADDING;
                hor_header_->ProcessEvent(ev_for_hor_header);
            }
            sf::Vector2f pos(event.touch.x / win_size.x, event.touch.y / win_size.y);
            if (std::abs(pos_.x + size_.x / 2 - pos.x) <= size_.x / 2 &&
                std::abs(pos_.y + size_.y / 2 - pos.y) <= size_.y / 2 &&
                (event.mouseButton.x - pos_.x * win_size.x - pos_in_.x - PADDING - ver_header_->Size()) / (cell_size.x * win_size.x) - 1 < machine_.Size().x &&
                (event.mouseButton.y - pos_.y * win_size.y - pos_in_.y - PADDING - hor_header_->Size()) / (cell_size.y * win_size.y) - 1 < machine_.Size().y &&
                can_change_text_) {
                active_pos_.emplace((event.mouseButton.x - pos_.x * win_size.x - pos_in_.x - PADDING - ver_header_->Size()) / cell_size.x - 1,
                                    (event.mouseButton.y - pos_.y * win_size.y - pos_in_.y - PADDING - hor_header_->Size()) / cell_size.y);
                std::cerr << active_pos_->y << std::endl;
                field.emplace(sf::Vector2f(active_pos_->x * cell_size.x + pos_in_.x + LINE_THICKNESS,
                                                active_pos_->y * cell_size.y + pos_in_.y + LINE_THICKNESS),
                              sf::Vector2f(cell_size.x - LINE_THICKNESS * 2, cell_size.y - LINE_THICKNESS * 2),
                              "Value");
                field->Activate();
            } else {
                field.reset();
            }
            break;
        }
        case sf::Event::MouseMoved:
            hovered_ =  std::abs(pos_.x + size_.x / 2 - event.mouseMove.x / win_size.x) <= size_.x / 2 &&
                        std::abs(pos_.y + size_.y / 2 - event.mouseMove.y / win_size.y) <= size_.y / 2;
            break;
        case sf::Event::TextEntered:
            if (field.has_value()) {
                if (event.text.unicode == 13) {
                    try {
                        machine_.SetValue(*active_pos_, field->GetText());
                    } catch (...) { }
                }
                else {
                    field->ProcessEvent(event);
                }
            }
            break;
    }
}

sf::Vector2i TableDrawer::GetTableSize() const {
    return { cell_size.x * machine_.Size().x, cell_size.y * machine_.Size().y };
}
