#include "core/physics/tree_drawer.h"
#include "core/trees/avl_tree.h"

#include "app/window_manager.h"

int main() {
    Tab tab;
    AvlSet<int64_t> avl_set;
    std::unique_ptr drawer(std::make_unique<TreesDrawer>(sf::Vector2f(10, 10), sf::Vector2f(980, 980), avl_set.GetRoot()));


}