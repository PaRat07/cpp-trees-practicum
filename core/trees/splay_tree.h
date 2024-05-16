#pragma once

#include "../physics/tree_drawer.h"

template<typename Val = int64_t, typename Cmp = std::less<>>
class SplaySet final : public TreeInterface<Val, Cmp> {
public:
    class Node final : public BaseNode {
    public:
        Node(const Val &val)
            : BaseNode(val)
        {}

        const BaseNode *GetLeft() const override {
            return left.get();
        }

        const BaseNode *GetRight() const override {
            return right.get();
        }

        std::string GetInfo() const override {
            return "Value: " + std::to_string(val);
        }

        std::unique_ptr<Node> left, right;
    };


    void Insert(const Val &key) override {
        std::lock_guard lock(operation_mutex_);
        if (root_ == nullptr) {
            root_ = std::make_unique<Node>(key);
            return;
        }
        Splay(root_, key);
        if (key < root_->val) {
            auto new_node = std::make_unique<Node>(key);
            new_node->left = std::move(root_->left);
            new_node->right = std::move(root_);
            root_ = std::move(new_node);
        } else if (key > root_->val) {
            auto new_node = std::make_unique<Node>(key);
            new_node->right = std::move(root_->right);
            new_node->left = std::move(root_);
            root_ = std::move(new_node);
        }
    }

    void Erase(const Val &val) override {
        std::lock_guard lock(operation_mutex_);
        if (root_ == nullptr) return;
        auto x = Find(root_, val);
        if (x == nullptr) return;
        Splay(root_, val);
        auto l = std::move(root_->left), r = std::move(root_->right);
        Splay(r, (*GetMin(r))->val);
        r->left = std::move(l);
        root_ = std::move(r);
    }

    void lock() override {
        drawing_mutex_.lock();
    }

    void unlock() override {
        drawing_mutex_.unlock();
    }

    const BaseNode *GetRoot() const override {
        return root_.get();
    }

private:

    static constexpr auto OPERATION_DELAY = std::chrono::milliseconds(1500 * 0);

    static void HoldMutex(std::unique_lock<std::mutex> lock) {
        std::this_thread::sleep_for(OPERATION_DELAY);
    }

    void Splay(std::unique_ptr<Node> &root, int64_t key) {
        if (root == nullptr || root->val == key) return;
        if (root->val > key) {
            if (root->left == nullptr) return;
            if (root->left->val > key) {
                Splay(root->left->left, key);
                Zig(root);
            } else if (root->left->val < key) {
                Splay(root->left->right, key);
                if (root->left->right!= nullptr) {
                    Zag(root->left);
                }
            }
            if (root->left != nullptr) {
                Zig(root);
            }
        } else {
            if (root->right == nullptr) return;
            if (root->right->val > key) {
                Splay(root->right->left, key);
                if (root->right->left!= nullptr) {
                    Zig(root->right);
                }
            } else if (root->right->val < key) {
                Splay(root->right->right, key);
                Zag(root);
            }
            if (root->right != nullptr) {
                Zag(root);
            }
        }
    }

    // 卐
    void Zig(std::unique_ptr<Node> &x) {
        std::unique_lock lock_turn(turn_mutex_);
        std::lock_guard lock_draw(drawing_mutex_);
        auto y = std::move(x->left);
        x->left = std::move(y->right);
        y->right = std::move(x);
        x = std::move(y);
        std::thread(HoldMutex, std::move(lock_turn)).detach();
    }

    // not-卐
    void Zag(std::unique_ptr<Node> &x) {
        std::unique_lock lock_turn(turn_mutex_);
        std::lock_guard lock_draw(drawing_mutex_);
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        y->left = std::move(x);
        x = std::move(y);
        std::thread(HoldMutex, std::move(lock_turn)).detach();
    }

    std::unique_ptr<Node> *Find(std::unique_ptr<Node> &root, int64_t key) {
        if (root == nullptr) return nullptr;
        if (root->val == key) return &root;
        if (root->val > key) {
            return Find(root->left, key);
        } else {
            return Find(root->right, key);
        }
    }

    std::unique_ptr<Node> *GetMin(std::unique_ptr<Node> &root) {
        if (root == nullptr) return nullptr;
        if (root->left == nullptr) return &root;
        return GetMin(root->left);
    }

    std::unique_ptr<Node> root_;
    std::mutex drawing_mutex_;
    std::mutex operation_mutex_;
    std::mutex turn_mutex_;
};