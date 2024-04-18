#include "table_header_drawer_interface.h"
#include "button.h"
#include "table_drawer.h"

struct EmptyHeader : public HeaderDrawer {
    float Size() const override { return 0; }
    void SetShift(float shift) override {}
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {}
    void ProcessEvent(sf::Event event) override {}
};

class VerticalHeader : public HeaderDrawer {
public:
    VerticalHeader(float width, Table &table)
        : width_(width)
        , table_(table)
    {
    }

    float Size() const override {
        return width_;
    }

    void SetShift(float shift) override {
        std::lock_guard lock(mutex_);
        std::for_each(buttons_.begin(), buttons_.end(), [&] (ButtonWithTextAbsPos &b) {
            b.SetPosition(b.GetPosition() + sf::Vector2f(0, shift_ - shift));
        });
        shift_ = shift;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        {
            std::lock_guard lock(mutex_);
            buttons_.clear();
            CenterPositionedString str;
            for (int i = 0; i < table_.Size().y; ++i) {
                buttons_.emplace_back(sf::Vector2f(20, TableDrawer::cell_size.y * i + TableDrawer::cell_size.y / 2 + shift_),
                                      sf::Vector2f(40, 20),
                                      "del",
                                      [&, id = i]() {
                                          table_.EraseLine(id);
                                      });
                str.setPosition(sf::Vector2f(50, TableDrawer::cell_size.y * i + TableDrawer::cell_size.y / 2 + 2 + shift_));
                str.setString(table_.GetLineName(i));
                target.draw(str);
            }
            std::for_each(buttons_.begin(), buttons_.end(), [&](const ButtonWithTextAbsPos &b) {
                target.draw(b);
            });
        }

        {
            sf::RectangleShape line(sf::Vector2f(width_, TableDrawer::LINE_THICKNESS));
            line.setFillColor(outline);
            for (int i = 0; i < table_.Size().y; ++i) {
                line.setPosition(0, i * TableDrawer::cell_size.y + shift_);
                target.draw(line);
            }
        }
    }

    void ProcessEvent(sf::Event event) override {
        std::lock_guard lock(mutex_);
        std::for_each(buttons_.begin(), buttons_.end(), [&event] (ButtonWithTextAbsPos &b) {
            b.ProcessEvent(event);
        });
    }

private:
    float width_;
    float shift_ = 0;
    Table &table_;
    mutable std::vector<ButtonWithTextAbsPos> buttons_;
    mutable std::mutex mutex_;
};