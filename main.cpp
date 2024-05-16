#include "core/physics/tree_drawer.h"
#include "core/trees/avl_tree.h"
#include "core/trees/rb_tree.h"
#include "core/trees/splay_tree.h"
#include "core/trees/cartesian_tree.h"
#include "drawers/button.h"
#include "app/window_manager.h"

sf::Vector2f win_size = {1000, 1000};
sf::Uint32 lambda(L'\u03BB');
long animation_time = 1500;


int main() {
    Tab tab;
    std::shared_ptr<AvlSet<>> empty_set = std::make_shared<AvlSet<>>();
    std::shared_ptr<RBTree> rb_set = std::make_shared<RBTree>();
    std::shared_ptr<SplaySet<>> splay_set = std::make_shared<SplaySet<>>();
    std::shared_ptr<AvlSet<>> avl_set = std::make_shared<AvlSet<>>();
    std::shared_ptr<CartesianSet> ct_set = std::make_shared<CartesianSet>();

    tab.AddElement(std::make_unique<ButtonWithTextRelativePos>(sf::Vector2f(5, 155), sf::Vector2f(190, 45), "Insert 20",
                                                        [&avl_set, &ct_set, &splay_set, rb_set] {
        std::thread([&avl_set, &ct_set, &splay_set, rb_set] {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            for (int i = 0; i < 50; ++i) {
                int new_elem = std::uniform_int_distribution<int>(1, 1000)(gen);

                std::thread avl_ins([&avl_set, new_elem] { avl_set->Insert(new_elem); });
                std::thread ct_ins([&ct_set, new_elem] { ct_set->Insert(new_elem); });
                std::thread splay_ins([&splay_set, new_elem] { splay_set->Insert(new_elem); });
                std::thread rb_ins([&rb_set, new_elem] { rb_set->Insert(new_elem); });
                avl_ins.join();
                ct_ins.join();
                splay_ins.join();
                rb_ins.join();
            }
        }).detach();
    }));

    {
        auto elem = std::make_unique<InputField>(sf::Vector2f(5, 5), sf::Vector2f(190, 45), "Value");
        tab.AddElement(std::make_unique<ButtonWithTextRelativePos>(sf::Vector2f(5, 105), sf::Vector2f(190, 45), "Insert",
            [new_elem = &*elem, &avl_set, &ct_set, &splay_set, rb_set] {
                try {
                    avl_set->Insert(std::stoll(new_elem->GetText()));
                    ct_set->Insert(std::stoll(new_elem->GetText()));
                    splay_set->Insert(std::stoll(new_elem->GetText()));
                    rb_set->Insert(std::stoll(new_elem->GetText()));
                } catch (...) {}
        }));

        tab.AddElement(std::move(elem));
    }

    tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f(200, 5), sf::Vector2f((1000 - 200 - 5 - 5) / 2, (1000 - 15) / 2), "AVL", avl_set));
    tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f((1000 - 200 - 5 - 5) / 2 + 200 + 5, 5), sf::Vector2f((1000 - 200 - 5 - 5) / 2, (1000 - 15) / 2), "RB", rb_set));
    tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f(200, (1000 - 15) / 2 + 10), sf::Vector2f((1000 - 200 - 5 - 5) / 2, (1000 - 15) / 2), "SPLAY", splay_set));
    tab.AddElement(std::make_unique<TreesDrawer>(sf::Vector2f((1000 - 200 - 5 - 5) / 2 + 200 + 5, (1000 - 15) / 2 + 10), sf::Vector2f((1000 - 200 - 5 - 5) / 2, (1000 - 15) / 2), "PIWO", ct_set));

    WindowManager wm(std::move(tab));
    wm.Start();
}
