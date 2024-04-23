#include <iostream>
#include <thread>
#include "../core/physics/tree_drawer.h"

TreesDrawer::Node::Node(int64_t v)
        : val(v)
{}

TreesDrawer::TreesDrawer(sf::Vector2f pos, sf::Vector2f size, const TreesDrawer::Node *root)
        : pos_(pos)
        , size_(size)
        , root_(root)
{}

void TreesDrawer::ProcessEvent(sf::Event event) {
    switch (event.type) {
        case sf::Event::MouseButtonPressed: {
            sf::Vector2f touch_pos(event.mouseButton.x - pos_.x, event.mouseButton.y - pos_.y);
            std::lock_guard lock(transaction_);
            auto nodes = AllNodes(root_);
            for (const auto &i: nodes) {
                if (CalcLength((i->pos - pos_in_) * zoom_ - touch_pos) <= RADIUS * std::sqrt(zoom_)) {
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
                grabbed_->pos = sf::Vector2f((event.mouseMove.x - pos_.x) / zoom_ + pos_in_.x, (event.mouseMove.y - pos_.y) / zoom_ + pos_in_.y);
            }
            break;
        }
        case sf::Event::MouseWheelScrolled: {
            float new_zoom = zoom_ * std::pow(0.9, event.mouseWheelScroll.delta);
            pos_in_ += (sf::Vector2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y) - pos_) / zoom_ - (sf::Vector2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y) - pos_) / new_zoom;
            zoom_ = new_zoom;
            break;
        }
    }
}

void TreesDrawer::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    std::lock_guard guard(transaction_);
    auto nodes = AllNodes(root_);
    for (const auto &i : nodes) {
        for (const auto &j : nodes) {
            if (i == j) continue;
            if (i->pos == j->pos) {
                i->pos += sf::Vector2f(5, 5);
            }
        }
    }
    DoPhysics(nodes);

    for (const auto &i : nodes) {
        if (i->GetLeft()) {
            LineShape line((i->pos - pos_in_) * zoom_ + pos_, (i->GetLeft()->pos - pos_in_) * zoom_ + pos_);
            line.setFillColor(sf::Color::Red);
            line.setThickness(5);
            target.draw(line);
        }
        if (i->GetRight()) {
            LineShape line((i->pos - pos_in_) * zoom_ + pos_, (i->GetRight()->pos - pos_in_) * zoom_ + pos_);
            line.setFillColor(sf::Color::Red);
            line.setThickness(5);
            target.draw(line);
        }
    }

    for (const auto &i : nodes) {
        {
            sf::CircleShape vertex(RADIUS * std::sqrt(zoom_));
            vertex.setPosition((i->pos - pos_in_) * zoom_ + pos_ - sf::Vector2f(RADIUS, RADIUS) * std::sqrt(zoom_));
            vertex.setFillColor(primary);
            target.draw(vertex);
        }
        {
            sf::Text str;
            str.setPosition((i->pos - pos_in_) * zoom_ + pos_ + sf::Vector2f(RADIUS, RADIUS) * std::sqrt(zoom_));
            str.setString(std::to_string(i->val));
            str.setFillColor(sf::Color::White);
            str.setCharacterSize(letter_size);
            str.setFont(font);
            target.draw(str);
        }
    }
}

void TreesDrawer::BeginTransaction() {
    transaction_.lock();
}

void TreesDrawer::EndTransaction(const TreesDrawer::Node *new_root) {
    root_ = new_root;
    transaction_.unlock();
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

std::vector<const TreesDrawer::Node*> TreesDrawer::AllNodes(const TreesDrawer::Node *root) {
    if (root == nullptr) return {};
    std::vector<const Node*> ans;
    {
        auto buf = AllNodes(root->GetLeft());
        std::copy(buf.begin(), buf.end(), std::back_inserter(ans));
    }
    ans.push_back(root);
    {
        auto buf = AllNodes(root->GetRight());
        std::copy(buf.begin(), buf.end(), std::back_inserter(ans));
    }
    return ans;
}

void TreesDrawer::DoPhysics(const std::vector<const Node *> &nodes) const {
    auto time = std::chrono::steady_clock::now();

    auto time_gone = std::chrono::duration_cast<std::chrono::milliseconds>(time - prev_draw_).count();
    time_gone /= 15;
    for (auto *i : nodes) {
        if (i == root_ || grabbed_ != nullptr && i == grabbed_) continue;

        sf::Vector2f acceleration(0, G_FOR_GRAVITY);

        const Node *par = nullptr;
        for (auto *j : nodes) {
            if (i == j) continue;
            if (j->GetLeft() == i || j->GetRight() == i) {
                par = j;
            }
            float force = M_OF_VERTEX * G_FOR_VERTEX / CalcSqLength(j->pos - i->pos);
            acceleration += sf::Vector2f(-(j->pos.x - i->pos.x) / CalcLength(j->pos - i->pos) * force,
                                         -(j->pos.y - i->pos.y) / CalcLength(j->pos - i->pos) * force);
        }
        // l child
        if (i->GetLeft() != nullptr) {
            acceleration += sf::Vector2f(-(i->pos.x - i->GetLeft()->pos.x) * (K_FOR_EDGES * GetSubtreeSize(i->GetLeft())) / M_OF_VERTEX,
                                         -(i->pos.y - i->GetLeft()->pos.y) * (K_FOR_EDGES * GetSubtreeSize(i->GetLeft())) / M_OF_VERTEX);
        }
        // r child
        if (i->GetRight() != nullptr) {
            acceleration += sf::Vector2f(-(i->pos.x - i->GetRight()->pos.x) * (K_FOR_EDGES * GetSubtreeSize(i->GetLeft())) / M_OF_VERTEX,
                                         -(i->pos.y - i->GetRight()->pos.y) * (K_FOR_EDGES * GetSubtreeSize(i->GetLeft())) / M_OF_VERTEX);
        }
        // par
        if (par != nullptr) {
            acceleration += sf::Vector2f(-(i->pos.x - par->pos.x) * (K_FOR_EDGES * GetSubtreeSize(i)) / M_OF_VERTEX,
                                         -(i->pos.y - par->pos.y) * (K_FOR_EDGES * GetSubtreeSize(i)) / M_OF_VERTEX);
        }
        if (par != nullptr) {
            if (par->GetLeft() == i) {
                acceleration.x -= G_FOR_CHILD_POWER * std::pow(GetSubtreeSize(i), 1.7);
            } else {
                acceleration.x += G_FOR_CHILD_POWER * std::pow(GetSubtreeSize(i), 1.7);
            }
        }

        i->speed += acceleration * time_gone;
        i->speed = i->speed * std::pow(0.95, time_gone);
        i->pos += i->speed * time_gone;
    }
    prev_draw_ = time;
}

size_t TreesDrawer::GetSubtreeSize(const TreesDrawer::Node *root) {
    if (root == nullptr) {
        return 0;
    }
    return 1 + GetSubtreeSize(root->GetLeft()) + GetSubtreeSize(root->GetRight());
}

sf::Vector2f TreesDrawer::GetRandomPoint() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist_x(0, win_size.x);
    std::uniform_int_distribution<int> dist_y(0, win_size.y);
    return sf::Vector2f(dist_x(gen), dist_y(gen));
}


