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
    tab.AddElement(std::make_unique<ButtonWithTextAbsPos>(sf::Vector2f(10, 10), sf::Vector2f(70, 45), "Add",
                                                        [&new_elem, &avl_set] {
        std::thread([&new_elem, &avl_set] {
            for (int i = 0; i < 10; ++i) avl_set->Insert(++new_elem);
        }).detach();
    }));
    tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f(200, 5), sf::Vector2f((1000 - 200 - 5 - 5) / 2, 500), "AVL", avl_set));
    tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f((1000 - 200 - 5 - 5) / 2 + 200 + 5, 5), sf::Vector2f((1000 - 200 - 5 - 5) / 2, 500), "RB", avl_set));
    // tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f(100, 100), sf::Vector2f(500, 500), "Avl", avl_set));
    // tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f(100, 100), sf::Vector2f(500, 500), "Avl", avl_set));

    WindowManager wm(std::move(tab));
    wm.Start();
}
