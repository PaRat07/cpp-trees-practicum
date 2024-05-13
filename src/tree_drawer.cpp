#include <iostream>
#include <thread>
#include "../core/physics/tree_drawer.h"


BaseNode::BaseNode(int64_t v)
        : val(v)
{}

TreesDrawer::TreesDrawer(sf::Vector2f pos, sf::Vector2f size, std::string title, std::shared_ptr<TreeInterface<>> tree)
        : pos_(pos.x / win_size.x, pos.y / win_size.y)
        , size_(size.x / win_size.x, size.y / win_size.y)
        , tree_(std::move(tree))
        , title_(std::move(title))
{}

void TreesDrawer::ProcessEvent(sf::Event event) {
    sf::Vector2f real_pos(pos_.x * win_size.x, pos_.y * win_size.y);
    sf::Vector2f real_size(size_.x * win_size.x, size_.y * win_size.y);
    if (erase_button_.has_value()) {
        erase_button_->ProcessEvent(event);
    }
    switch (event.type) {
        case sf::Event::MouseButtonPressed: {
            if (std::abs(event.mouseButton.x - (real_pos.x + real_size.x / 2)) > real_size.x / 2 ||
                std::abs(event.mouseButton.y - (real_pos.y + real_size.y / 2)) > real_size.y / 2) {
                break;
                }
            sf::Vector2f touch_pos(event.mouseButton.x - real_pos.x, event.mouseButton.y - real_pos.y);
            std::lock_guard lock(*tree_);
            auto nodes = AllNodes(tree_->GetRoot());
            active_node_ = nullptr;
            erase_button_.reset();
            for (const auto &i: nodes) {
                if (CalcLength((i->pos - pos_in_) * zoom_ - touch_pos) <= RADIUS * std::sqrt(zoom_)) {
                    grabbed_ = i;
                    active_node_ = i;
                    erase_button_.emplace(real_pos + sf::Vector2f(real_size.x - 115, 80), sf::Vector2f(100,  40), "Erase", [&] {
                        int64_t val = active_node_->val;
                        active_node_ = nullptr;
                        erase_button_.reset();
                        tree_->Erase(val);
                    });
                }
            }
            if (grabbed_ == nullptr) {
                grabbed_pos_in_.emplace(event.mouseButton.x, event.mouseButton.y);
            }
            tree_->unlock();
            break;
        }
        case sf::Event::MouseButtonReleased: {
            grabbed_ = nullptr;
            grabbed_pos_in_.reset();
            break;
        }
        case sf::Event::MouseMoved: {
            if (grabbed_ != nullptr) {
                grabbed_->pos = sf::Vector2f((event.mouseMove.x - real_pos.x) / zoom_ + pos_in_.x, (event.mouseMove.y - real_pos.y) / zoom_ + pos_in_.y);
            } else if (grabbed_pos_in_) {
                pos_in_ -= sf::Vector2f((event.mouseMove.x - grabbed_pos_in_->x) / zoom_, (event.mouseMove.y - grabbed_pos_in_->y) / zoom_);
                grabbed_pos_in_.emplace(event.mouseMove.x, event.mouseMove.y);
            }
            break;
        }
        case sf::Event::MouseWheelScrolled: {
            if (std::abs(event.mouseWheelScroll.x - (real_pos.x + real_size.x / 2)) > real_size.x / 2 ||
                std::abs(event.mouseWheelScroll.y - (real_pos.y + real_size.y / 2)) > real_size.y / 2) {
                break;
            }
            float new_zoom = zoom_ * std::pow(0.9, event.mouseWheelScroll.delta);
            pos_in_ += (sf::Vector2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y) - real_pos) / zoom_ - (sf::Vector2f(event.mouseWheelScroll.x, event.mouseWheelScroll.y) - real_pos) / new_zoom;
            zoom_ = new_zoom;
            break;
        }
        case sf::Event::TextEntered: {
            // ctrl + h
            if (event.text.unicode == 8) {
                pos_in_ = tree_->GetRoot()->pos - real_size * 0.5;
            }
        }
    }
}

void TreesDrawer::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    sf::Vector2f real_pos(pos_.x * win_size.x, pos_.y * win_size.y);
    sf::Vector2f real_size(size_.x * win_size.x, size_.y * win_size.y);
    std::lock_guard lock(*tree_);
    auto nodes = AllNodes(tree_->GetRoot());
    for (const auto &i : nodes) {
        for (const auto &j : nodes) {
            if (i == j) continue;
            if (i->pos == j->pos) {
                i->pos += sf::Vector2f(5, 5);
            }
        }
    }

    sf::RenderTexture texture;
    texture.create(real_size.x, real_size.y);
    texture.clear(surface);
    {
        RoundedRectangleShape rect(sf::Vector2f(real_size.x, real_size.y));
        rect.setFillColor(surface_container);
        rect.setRoundRadius(10);
        rect.setPosition(sf::Vector2f(0, 0));
        texture.draw(rect);
    }

    for (const auto &i : nodes) {
        if (i->GetLeft()) {
            LineShape line((i->pos - pos_in_) * zoom_, (i->GetLeft()->pos - pos_in_) * zoom_);
            line.setFillColor(sf::Color::Red);
            line.setThickness(5);
            texture.draw(line);
        }
        if (i->GetRight()) {
            LineShape line((i->pos - pos_in_) * zoom_, (i->GetRight()->pos - pos_in_) * zoom_);
            line.setFillColor(sf::Color::Red);
            line.setThickness(5);
            texture.draw(line);
        }
    }

    for (const auto &i : nodes) {
        {
            sf::CircleShape vertex(RADIUS * std::sqrt(zoom_));
            vertex.setPosition((i->pos - pos_in_) * zoom_ - sf::Vector2f(RADIUS, RADIUS) * std::sqrt(zoom_));
            vertex.setFillColor(primary);
            vertex.setPointCount(100);
            texture.draw(vertex);
        }
        {
            sf::Text str;
            str.setPosition((i->pos - pos_in_) * zoom_ + sf::Vector2f(RADIUS, RADIUS) * std::sqrt(zoom_));
            str.setString(std::to_string(i->val));
            str.setFillColor(sf::Color::White);
            str.setCharacterSize(12);
            str.setFont(font);
            texture.draw(str);
        }
    }
    if (active_node_ != nullptr) {
        {
            RoundedRectangleShape rect(sf::Vector2f(150, 120));
            rect.setFillColor(surface_container_highest);
            rect.setRoundRadius(10);
            rect.setPosition(sf::Vector2f(real_size.x - 155, 5));
            texture.draw(rect);
        }
        {
            sf::Text info;
            info.setPosition(real_size.x - 145, 5);
            info.setString(active_node_->GetInfo());
            info.setFillColor(sf::Color::White);
            info.setCharacterSize(15);
            info.setFont(font);
            texture.draw(info);
        }
    }
    sf::Sprite sprite;
    texture.display();
    sprite.setTexture(texture.getTexture());
    sprite.setPosition(real_pos);
    target.draw(sprite);
    if (erase_button_.has_value()) {
        erase_button_.emplace(real_pos + sf::Vector2f(real_size.x - 145, 75), sf::Vector2f(130,  40), "Erase", [&] {
                        int64_t val = active_node_->val;
                        active_node_ = nullptr;
                        erase_button_.reset();
                        tree_->Erase(val);
                    });
        target.draw(*erase_button_);
    }
    {
        sf::Text title;
        title.setPosition(real_pos + sf::Vector2f(5, 5));
        title.setString(title_);
        title.setFillColor(on_surface);
        title.setCharacterSize(30);
        title.setFont(font);
        target.draw(title);
    }
    DoPhysics(std::move(nodes));
    // TODO: add multithreading
    // std::thread(DoPhysics, std::move(nodes)).detach();
}

std::vector<const BaseNode*> TreesDrawer::AllNodes(const BaseNode *root) {
    if (root == nullptr) return {};
    std::vector<const BaseNode*> ans;
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

void TreesDrawer::DoPhysics(std::vector<const BaseNode *> nodes) const {
    // std::lock_guard lock(*tree_);
    auto time = std::chrono::steady_clock::now();

    auto time_gone = std::chrono::duration_cast<std::chrono::milliseconds>(time - prev_draw_).count();
    time_gone /= 15;
    for (auto *i : nodes) {
        if (i == tree_->GetRoot() || i == grabbed_) continue;

        sf::Vector2f acceleration(0, G_FOR_GRAVITY);

        const BaseNode *par = nullptr;
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
            acceleration += sf::Vector2f(-(i->pos.x - i->GetLeft()->pos.x) * (K_FOR_EDGES * GetSubtreeSize(i->GetLeft()) / (0.05 * pow(nodes.size(), 1))) / M_OF_VERTEX,
                                         -(i->pos.y - i->GetLeft()->pos.y) * (K_FOR_EDGES * GetSubtreeSize(i->GetLeft()) / (0.05 * pow(nodes.size(), 1))) / M_OF_VERTEX);
        }
        // r child
        if (i->GetRight() != nullptr) {
            acceleration += sf::Vector2f(-(i->pos.x - i->GetRight()->pos.x) * (K_FOR_EDGES * GetSubtreeSize(i->GetRight()) / (0.05 * pow(nodes.size(), 1))) / M_OF_VERTEX,
                                         -(i->pos.y - i->GetRight()->pos.y) * (K_FOR_EDGES * GetSubtreeSize(i->GetRight()) / (0.05 * pow(nodes.size(), 1))) / M_OF_VERTEX);
        }
        // par
        if (par != nullptr) {
            acceleration += sf::Vector2f(-(i->pos.x - par->pos.x) * (K_FOR_EDGES * pow(GetSubtreeSize(i), 1) / (0.05 * pow(nodes.size(), 1))) / M_OF_VERTEX,
                                         -(i->pos.y - par->pos.y) * (K_FOR_EDGES * pow(GetSubtreeSize(i), 1) / (0.05 * pow(nodes.size(), 1))) / M_OF_VERTEX);
        }
        if (par != nullptr) {
            if (par->GetLeft() == i) {
                acceleration.x -= G_FOR_CHILD_POWER * std::pow(GetSubtreeSize(i) + 1, 2.3);
            } else {
                acceleration.x += G_FOR_CHILD_POWER * std::pow(GetSubtreeSize(i) + 1, 2.3);
            }
        }

        i->speed += acceleration * time_gone;
        i->speed = i->speed * std::pow(0.95, time_gone);
        i->pos += i->speed * time_gone;
    }
    prev_draw_ = time;
}

size_t TreesDrawer::GetSubtreeSize(const BaseNode *root) {
    if (root == nullptr) {
        return 0;
    }
    return 1 + GetSubtreeSize(root->GetLeft()) + GetSubtreeSize(root->GetRight());
}

sf::Vector2f BaseNode::GetRandomPoint() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist_x( 0, win_size.x);
    std::uniform_int_distribution<int> dist_y(0, win_size.y);
    return sf::Vector2f(dist_x(gen), dist_y(gen));
}


