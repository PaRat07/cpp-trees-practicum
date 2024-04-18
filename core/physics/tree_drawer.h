#pragma once

#include <memory>
#include <chrono>
#include <cmath>

#include "../../app/tab.h"
#include "vector_operations.h"
#include "../../drawers/line.h"

class TreesDrawer : public AbstractDrawer {
public:
    static constexpr float RADIUS = 50;
    static constexpr float K_FOR_EDGES = 10;
    static constexpr float G_FOR_GRAVITY = 10;
    static constexpr float G_FOR_VERTEX = 10;
    static constexpr float M_OF_VERTEX = 10;

    struct Node : public AbstractDrawer {
        friend class TreesDrawer;
        virtual const std::unique_ptr<Node> &GetLeft() const = 0;
        virtual const std::unique_ptr<Node> &GetRight() const = 0;
        virtual const Node *GetParent() const = 0;
        int64_t val;
        sf::Color color;

    private:
        mutable sf::Vector2f pos;
    };

    TreesDrawer(sf::Vector2f pos, sf::Vector2f size, const std::unique_ptr<Node> &root)
        : pos_(pos)
        , size_(size)
        , root_(root)
    {}

    void ProcessEvent(sf::Event event) {
        switch (event.type) {
            case sf::Event::MouseButtonPressed: {
                auto nodes = AllNodes(root_);
                for (const auto &i: nodes) {
                    if (CalcLength((*i)->pos - sf::Vector2f(event.mouseButton.x - pos_.x, event.mouseButton.y - pos_.y)) <= RADIUS) {
                        grabbed_ = i;
                    }
                }
                break;
            }
            case sf::Event::MouseButtonReleased: {
                grabbed_ = nullptr;
                break;
            }
            case sf::Event::MouseMoved: {
                if (grabbed_ != nullptr) {
                    (*grabbed_)->pos = sf::Vector2f(event.mouseMove.x - pos_.x, event.mouseMove.y - pos_.y);
                }
                break;
            }
        }
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        transaction_.lock();
        auto nodes = AllNodes(root_);
        DoPhysics(nodes);

        for (const auto &i : nodes) {
            {
                sf::CircleShape vertex(RADIUS);
                vertex.setPosition((*i)->pos);
                target.draw(vertex);
            }
            if ((*i)->GetLeft()){
                LineShape line((*i)->pos, (*i)->GetLeft()->pos);
                line.setFillColor(sf::Color::Red);
                target.draw(line);
            }
            if ((*i)->GetRight()) {
                LineShape line((*i)->pos, (*i)->GetRight()->pos);
                line.setFillColor(sf::Color::Red);
                target.draw(line);
            }
        }
    }

    void BeginTransaction() {
        transaction_.lock();
    }

    void EndTransaction() {
        transaction_.unlock();
    }

private:
    sf::Vector2f pos_, size_;
    const std::unique_ptr<Node> &root_;
    mutable std::chrono::steady_clock::time_point prev_draw_;
    const std::unique_ptr<Node> *grabbed_ = nullptr;
    mutable std::mutex transaction_;

    static std::vector<const std::unique_ptr<Node>*> AllNodes(const std::unique_ptr<Node> &root) {
        if (root == nullptr) return {};
        std::vector<const std::unique_ptr<Node>*> ans;
        {
            auto buf = AllNodes(root->GetRight());
            std::copy(buf.begin(), buf.end(), std::back_inserter(ans));
        }
        ans.emplace_back(&root);
        {
            auto buf = AllNodes(root->GetRight());
            std::copy(buf.begin(), buf.end(), std::back_inserter(ans));
        }
        return ans;
    }

    void DoPhysics(const std::vector<const std::unique_ptr<Node>*> &nodes) const {
        auto time = std::chrono::steady_clock::now();

        auto time_gone = std::chrono::duration_cast<std::chrono::milliseconds>(time - prev_draw_).count();
        for (auto *i : nodes) {
            if (*i == root_ || grabbed_ != nullptr && *i == *grabbed_) continue;

            sf::Vector2f acceleration(0, G_FOR_GRAVITY);
            for (auto *j : nodes) {
                float force = M_OF_VERTEX * G_FOR_VERTEX / CalcSqLength((*j)->pos - (*i)->pos);
                acceleration += sf::Vector2f(-CalcLength((*j)->pos - (*i)->pos) / ((*j)->pos.x - (*i)->pos.x) * force,
                                             -CalcLength((*j)->pos - (*i)->pos) / ((*j)->pos.y - (*i)->pos.y) * force);
            }
            // l child
            {
                float force = CalcLength((*i)->GetLeft()->pos - (*i)->pos) * K_FOR_EDGES;
                acceleration += sf::Vector2f(((*i)->pos.x - (*i)->GetLeft()->pos.x) / CalcLength((*i)->GetLeft()->pos - (*i)->pos) * force,
                                             ((*i)->pos.y - (*i)->GetLeft()->pos.y) / CalcLength((*i)->GetLeft()->pos - (*i)->pos) * force);
            }
            // r child
            {
                float force = CalcLength((*i)->GetRight()->pos - (*i)->pos) * K_FOR_EDGES;
                acceleration += sf::Vector2f(((*i)->pos.x - (*i)->GetRight()->pos.x) / CalcLength((*i)->GetRight()->pos - (*i)->pos) * force,
                                             ((*i)->pos.y - (*i)->GetRight()->pos.y) / CalcLength((*i)->GetRight()->pos - (*i)->pos) * force);
            }
            // par
            {
                float force = CalcLength((*i)->GetRight()->pos - (*i)->pos) * K_FOR_EDGES;
                acceleration += sf::Vector2f(((*i)->pos.x - (*i)->GetParent()->pos.x) / CalcLength((*i)->GetParent()->pos - (*i)->pos) * force,
                                             ((*i)->pos.y - (*i)->GetParent()->pos.y) / CalcLength((*i)->GetParent()->pos - (*i)->pos) * force);
            }
            (*i)->pos += acceleration * time_gone;
        }
        prev_draw_ = time;
    }
};
