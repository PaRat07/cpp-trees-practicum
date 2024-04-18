#pragma once

#include "SFML/Graphics.hpp"

#include "../app/tab.h"
#include "table_interface.h"
#include "rounded_rectangle.h"
#include "../app/colors.h"
#include "table_header_drawer_interface.h"

class TableDrawer : public AbstractDrawer {
public:

    static sf::Vector2i cell_size;

    TableDrawer(sf::Vector2f pos,
                sf::Vector2f size,
                Table &machine,
                bool allow_change_text,
                std::unique_ptr<HeaderDrawer> hor_header,
                std::unique_ptr<HeaderDrawer> ver_header);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    virtual void ProcessEvent(sf::Event event) override;

    static constexpr int PADDING = 10;
    static constexpr int LINE_THICKNESS = 2;

private:
    Table &machine_;
    sf::Vector2f pos_, size_;
    sf::Vector2i pos_in_ = {0,0};

    std::optional<sf::Vector2i> active_pos_;
    std::optional<InputField> field;

    std::unique_ptr<HeaderDrawer> hor_header_, ver_header_;

    bool hovered_ = false;
    bool can_change_text_;

    sf::Vector2i GetTableSize() const;
};
