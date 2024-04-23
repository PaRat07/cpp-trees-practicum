#pragma once

#include <memory>
#include <chrono>
#include <cmath>
#include <random>

#include "../../app/tab.h"
#include "vector_operations.h"
#include "../../drawers/center_positioned_string.h"
#include "../../drawers/line.h"

class TreesDrawer : public AbstractDrawer {
public:

    struct Node : public AbstractDrawer {
        Node(int64_t v);

        friend class TreesDrawer;
        virtual const Node *GetLeft() const = 0;
        virtual const Node *GetRight() const = 0;
        int64_t val;
        sf::Color color;

    private:
        mutable sf::Vector2f speed = {0, 0};
        mutable sf::Vector2f pos = GetRandomPoint();
    };

    TreesDrawer(sf::Vector2f pos, sf::Vector2f size, const Node *root);

    void ProcessEvent(sf::Event event) override;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void BeginTransaction();

    void EndTransaction(const Node *new_root);

private:
    float zoom_ = 1;
    sf::Vector2f pos_, size_, pos_in_;
    const Node *root_;
    const Node *grabbed_ = nullptr;
    mutable std::mutex transaction_;
    mutable std::chrono::steady_clock::time_point prev_draw_;

    static constexpr float RADIUS = 50;
    static constexpr float K_FOR_EDGES = 50;
    static constexpr float G_FOR_GRAVITY = 10;
    static constexpr float G_FOR_CHILD_POWER = 4;
    static constexpr float G_FOR_EDGES = 5;
    static constexpr float G_FOR_VERTEX = 10;
    static constexpr float M_OF_VERTEX = 1e4;

    static std::vector<const Node*> AllNodes(const Node *root);
    static size_t GetSubtreeSize(const Node *root);
    static sf::Vector2f GetRandomPoint();

    void DoPhysics(const std::vector<const Node*> &nodes) const;
};
