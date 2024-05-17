#pragma once

#include <memory>
#include <chrono>
#include <cmath>
#include <random>
#include <functional>
#include <mutex>

#include "../../app/tab.h"
#include "vector_operations.h"
#include "../../drawers/button.h"
#include "../../drawers/center_positioned_string.h"
#include "../../drawers/line.h"

struct BaseNode {
    BaseNode(int64_t v);

    friend class TreesDrawer;
    virtual const BaseNode *GetLeft() const = 0;
    virtual const BaseNode *GetRight() const = 0;
    virtual std::string GetInfo() const = 0;
    int64_t val;
    sf::Color color = primary;

private:
    mutable sf::Vector2<double> speed = {0, 0};
    mutable sf::Vector2<double> pos = GetRandomPoint();
    static sf::Vector2<double> GetRandomPoint();
};

template<typename Val = int64_t, typename Cmp = std::less<>>
class TreeInterface {
public:
    virtual void Insert(const Val &val) = 0;
    virtual void Erase(const Val &val) = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual const BaseNode *GetRoot() const = 0;
};

class TreesDrawer final : public AbstractDrawer {
public:
    TreesDrawer(sf::Vector2f pos, sf::Vector2f size, std::string title, std::shared_ptr<TreeInterface<>> tree);

    void ProcessEvent(sf::Event event) override;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::string title_;
    float zoom_ = 0.1;
    sf::Vector2f pos_, size_, pos_in_;
    std::shared_ptr<TreeInterface<>> tree_;
    mutable std::optional<ButtonWithTextAbsPos> erase_button_;
    std::optional<sf::Vector2f> grabbed_pos_in_;
    mutable const BaseNode* active_node_ = nullptr;
    const BaseNode *grabbed_ = nullptr;
    mutable std::chrono::steady_clock::time_point prev_draw_;

    static constexpr float RADIUS = 30;
    static constexpr float K_FOR_EDGES = 40;
    static constexpr float G_FOR_GRAVITY = 10;
    static constexpr float G_FOR_CHILD_POWER = 1.5;
    static constexpr float G_FOR_VERTEX = 300;
    static constexpr float M_OF_VERTEX = 1e4;

    static std::vector<const BaseNode*> AllNodes(const BaseNode *root);
    static size_t GetSubtreeSize(const BaseNode *root);

    void DoPhysics(std::vector<const BaseNode *> nodes) const;
};
