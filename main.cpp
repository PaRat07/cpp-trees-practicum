#include "core/physics/tree_drawer.h"
#include "core/trees/avl_tree.h"
#include "drawers/button.h"
#include "app/window_manager.h"

sf::Vector2f win_size = {1000, 1000};
sf::Uint32 lambda(L'\u03BB');
long animation_time = 1500;

int main() {
    Tab tab;
    std::shared_ptr<AvlSet<>> avl_set = std::make_shared<AvlSet<>>();
    int new_elem = 0;
    tab.AddElement(std::make_unique<ButtonWithTextRelativePos>(sf::Vector2f(5, 155), sf::Vector2f(190, 45), "Insert 10",
                                                        [&new_elem, &avl_set] {
        std::thread([&new_elem, &avl_set] {
            for (int i = 0; i < 10; ++i) avl_set->Insert(++new_elem);
        }).detach();
    }));
    {
        auto elem = std::make_unique<InputField>(sf::Vector2f(5, 5), sf::Vector2f(190, 45), "Value");
        auto mt_val = std::make_unique<InputField>(sf::Vector2f(5, 55), sf::Vector2f(190, 45), "mt19937");
        tab.AddElement(std::make_unique<ButtonWithTextRelativePos>(sf::Vector2f(5, 105), sf::Vector2f(185 / 2, 45), "Insert",
            [new_elem = &*elem, &avl_set, mt_val = &mt_val] {
                avl_set->Insert(std::stoll(new_elem->GetText()));
        }));
        tab.AddElement(std::make_unique<ButtonWithTextRelativePos>(sf::Vector2f(10 + 185 / 2, 105), sf::Vector2f(185 / 2, 45), "Erase",
            [new_elem = &*elem, &avl_set, mt_val = &mt_val] {
                avl_set->Insert(std::stoll(new_elem->GetText()));
            }));
        tab.AddElement(std::move(elem));
        tab.AddElement(std::move(mt_val));
    }

    tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f(200, 5), sf::Vector2f((1000 - 200 - 5 - 5) / 2, (1000 - 15) / 2), "AVL", avl_set));
    tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f((1000 - 200 - 5 - 5) / 2 + 200 + 5, 5), sf::Vector2f((1000 - 200 - 5 - 5) / 2, (1000 - 15) / 2), "RB", avl_set));
    tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f(200, (1000 - 15) / 2 + 10), sf::Vector2f((1000 - 200 - 5 - 5) / 2, (1000 - 15) / 2), "SPLAY", avl_set));
    tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f((1000 - 200 - 5 - 5) / 2 + 200 + 5, (1000 - 15) / 2 + 10), sf::Vector2f((1000 - 200 - 5 - 5) / 2, (1000 - 15) / 2), "PIWO", avl_set));

    WindowManager wm(std::move(tab));
    wm.Start();
}
