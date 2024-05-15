#pragma once

#include "../physics/tree_drawer.h"

template<typename Val = int64_t, typename Cmp = std::less<>>
class SplaySet : public TreeInterface<Val, Cmp> {
public:
    class Node : public BaseNode {
    public:
        Node(const Val &val)
            : BaseNode(val)
        {}

        const BaseNode *GetLeft() const override {
            return left_.get();
        }

        const BaseNode *GetRight() const override {
            return right_.get();
        }

        std::string GetInfo() const override {
            return "Value: " + std::to_string(val);
        }

        std::unique_ptr<Node> left_, right_;
        std::unique_ptr<Node> *parent_ = nullptr;
    };


    void Insert(const Val &val) override {
        std::lock_guard lock(mutex_);
        assert(root_ == nullptr || root_->parent_ == nullptr);
        auto [l, r] = Split(std::move(root_), val);
        if (r == nullptr || r->val != val) {
            root_ = std::make_unique<Node>(val);
            root_->left_ = std::move(l);
            root_->right_ = std::move(r);
            UpdChildren(root_);
        } else {
            root_ = Merge(std::move(l), std::move(r));
        }
    }

    void Erase(const Val &val) override {
        // FIXME
        throw std::runtime_error("Not implemented");
    }

    void lock() override {
        mutex_.lock();
    }

    void unlock() override {
        mutex_.unlock();
    }

    const BaseNode *GetRoot() const override {
        return root_.get();
    }

private:

    void Splay(std::unique_ptr<Node> &node) {
        if (node->parent_ == nullptr) return;
        if ((*node->parent_)->parent_ == nullptr) {
            if ((*node->parent_)->left_ == node) {
                Zig(*node->parent_);
            } else {
                Zag(*node->parent_);
            }
        } else {
            if ((*node->parent_)->left_ == node) {
                if ((*(*node->parent_)->parent_)->left_ == *node->parent_) {
                    Zig(*(*node->parent_)->parent_);
                    Zig(*node->parent_);
                } else {
                    Zig(*node->parent_);
                    Zag(*(*node->parent_)->parent_);
                }
            } else {
                if ((*(*node->parent_)->parent_)->left_ == *node->parent_) {
                    Zag(*node->parent_);
                    Zig(*(*node->parent_)->parent_);
                } else {
                    Zag(*(*node->parent_)->parent_);
                    Zag(*node->parent_);
                }
            }
            Splay(*(*node->parent_)->parent_);
        }
    }

    // 卐
    void Zig(std::unique_ptr<Node> &node) {
        auto tmp = std::move(node);
        node = std::move(tmp->left_);
        node->parent_ = tmp->parent_;
        tmp->left_ = std::move(node->right_);
        UpdChildren(tmp);
        UpdChildren(tmp->left_);
        UpdChildren(tmp->right_);
        node->right_ = std::move(tmp);
        UpdChildren(node);
        UpdChildren(node->left_);
        UpdChildren(node->right_);
    }

    // not-卐
    void Zag(std::unique_ptr<Node> &node) {
        auto tmp = std::move(node);
        node = std::move(tmp->right_);
        node->parent_ = tmp->parent_;
        tmp->right_ = std::move(node->left_);
        UpdChildren(tmp);
        UpdChildren(tmp->left_);
        UpdChildren(tmp->right_);
        node->left_ = std::move(tmp);
        UpdChildren(node);
        UpdChildren(node->left_);
        UpdChildren(node->right_);
    }

    std::pair<std::unique_ptr<Node>, std::unique_ptr<Node>> Split(std::unique_ptr<Node> node, const Val &val) {
        if (node == nullptr) {
            return {nullptr, nullptr};
        }
        Splay(Find(node, val));
        std::unique_ptr<Node> l, r;
        if (node->val < val) {
            r = std::move(node->right_);
            l = std::move(node);
        } else {
            l = std::move(node->left_);
            r = std::move(node);
        }
        if (r != nullptr) {
            r->parent_ = nullptr;
        }
        if (l != nullptr) {
            l->parent_ = nullptr;
        }
        UpdChildren(l);
        UpdChildren(r);
        return { std::move(l), std::move(r) };
    }

    std::unique_ptr<Node> Merge(std::unique_ptr<Node> left, std::unique_ptr<Node> right) {
        Splay(GetMax(left));
        left->right_ = std::move(right);
        UpdChildren(left);
        return left;
    }

    static std::unique_ptr<Node> &Find(std::unique_ptr<Node> &node, const Val &val) {
        if (node->val == val) {
            return node;
        } else if (node->val < val) {
            return node->right_ ? Find(node->right_, val) : node;
        } else {
            return node->left_ ? Find(node->left_, val) : node;
        }
    }

    static std::unique_ptr<Node> &GetMax(std::unique_ptr<Node> &node) {
        if (node->right_ == nullptr) {
            return node;
        } else {
            return GetMax(node->right_);
        }
    }

    static void UpdChildren(std::unique_ptr<Node> &node) {
        if (node == nullptr) return;

        if (node->left_ != nullptr) {
            node->left_->parent_ = &node;
        }
        if (node->right_!= nullptr) {
            node->right_->parent_ = &node;
        }
    }

    std::unique_ptr<Node> root_;
    std::mutex mutex_;
};