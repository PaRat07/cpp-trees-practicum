#include "core/physics/tree_drawer.h"
#include "core/trees/avl_tree.h"
#include "drawers/button.h"
#include "app/window_manager.h"

sf::Vector2f win_size = {1000, 1000};
sf::Uint32 lambda(L'\u03BB');
long animation_time = 1500;

int main() {
    Tab tab;
    AvlSet avl_set;
    auto drawer = std::make_unique<TreesDrawer>(sf::Vector2f(10, 10), sf::Vector2f(980, 980), reinterpret_cast<const TreesDrawer::Node*>(avl_set.GetRoot().get()));
    int new_elem = 0;
    avl_set.SetCallBacks([&drawer = *drawer] () {
        drawer.BeginTransaction();
    },
[&drawer = *drawer] (const AvlSet<>::AvlNode *new_root) {
        drawer.EndTransaction(reinterpret_cast<const TreesDrawer::Node*>(new_root));
    });
    tab.AddElement(std::make_unique<ButtonWithTextAbsPos>(sf::Vector2f(10, 10), sf::Vector2f(70, 45), "Add",
                                                        [&new_elem, &drawer = *drawer, &avl_set] () {
        std::thread([&] () {
            for (int i = 0; i < 10; ++i) avl_set.Insert(++new_elem);
        }).detach();
    }));
    tab.AddElement(std::move(drawer));

    WindowManager wm(std::move(tab));
    wm.Start();
}
