#include "../app/tab.h"

void Tab::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    std::for_each(elements_.begin(), elements_.end(), [&target] (const std::unique_ptr<AbstractDrawer> &ptr) {
        target.draw(*ptr);
    });
}

void Tab::ProcessEvent(sf::Event event) {
    std::for_each(elements_.begin(), elements_.end(), [event] (const std::unique_ptr<AbstractDrawer> &ptr) {
        ptr->ProcessEvent(event);
    });
}
