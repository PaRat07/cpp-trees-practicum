#include "../physics/tree_drawer.h"
#include "../../drawers/button.h"
#include "../../drawers/inoutput_field.h"

#include <functional>
#include <mutex>
#include <thread>


template<class Val = int64_t, class Cmp = std::less<>>
class AvlSet : public TreeInterface<Val, Cmp> {
public:
    struct AvlNode : public BaseNode {
        AvlNode(int64_t val)
            : BaseNode(val)
        {
        }

        const BaseNode *GetLeft() const override {
            return left.get();
        }
        const BaseNode *GetRight() const override {
            return right.get();
        }

        std::string GetInfo() const override {
            return "Height: " + std::to_string(height);
        }

        int height = 1;
        std::unique_ptr<AvlNode> left, right;
    };

    void Insert(const Val &v) override {
        std::lock_guard lock(operation_mutex_);
        Insert(root_, std::move(v));
    }

    void Erase(const Val &v) override {
        std::lock_guard lock(operation_mutex_);
        std::cerr << "Erase: " << v << std::endl;
        Erase(root_, v);
    }

    const BaseNode *GetRoot() const {
        return dynamic_cast<const BaseNode*>(root_.get());
    }

    void lock() override {
        drawing_mutex_.lock();
    }

    void unlock() override {
        drawing_mutex_.unlock();
    }

private:
    std::unique_ptr<AvlNode> root_;
    std::mutex drawing_mutex_;
    std::mutex operation_mutex_;
    std::mutex turn_mutex_;

    static constexpr auto OPERATION_DELAY = std::chrono::milliseconds(1500 * 0);

    static const std::unique_ptr<AvlNode> &Begin(const std::unique_ptr<AvlNode> &root) {
        return root->left ? Begin(root->left) : root;
    }

    static std::unique_ptr<AvlNode> &Begin(std::unique_ptr<AvlNode> &root) {
        return root->left ? Begin(root->left) : root;
    }

    static void UpdH(std::unique_ptr<AvlNode> &root) {
        root->height = 1;
        if (root->left) {
            root->height = std::max(root->height, root->left->height + 1);
        }
        if (root->right) {
            root->height = std::max(root->height, root->right->height + 1);
        }
    }

    static int GetH(const std::unique_ptr<AvlNode> &v) {
        return v ? v->height : 0;
    }

    static int Diff(const std::unique_ptr<AvlNode> &v) {
        return v ? GetH(v->left) - GetH(v->right) : 0;
    }

    static void HoldMutex(std::unique_lock<std::mutex> &&lock) {
        std::this_thread::sleep_for(OPERATION_DELAY);
    }

    void Rebalance(std::unique_ptr<AvlNode> &root) {
        std::unique_lock lock_turn(turn_mutex_);
        std::lock_guard lock_draw(drawing_mutex_);
        std::unique_ptr<AvlNode> root_m = std::move(root);
        if (Diff(root_m) == 2) {
            if (Diff(root_m->left) == -1) {
                std::unique_ptr<AvlNode> new_root = std::move(root_m->left->right);
                root_m->left->right = std::move(new_root->left);
                new_root->left = std::move(root_m->left);
                root_m->left = std::move(new_root->right);
                new_root->right = std::move(root_m);
                UpdH(new_root->left);
                UpdH(new_root->right);
                UpdH(new_root);
                root = std::move(new_root);
                std::thread(HoldMutex, std::move(lock_turn)).detach();
            } else {
                std::unique_ptr<AvlNode> new_root = std::move(root_m->left);
                root_m->left = std::move(new_root->right);
                new_root->right = std::move(root_m);
                UpdH(new_root->left);
                UpdH(new_root->right);
                UpdH(new_root);
                root = std::move(new_root);
                std::thread(HoldMutex, std::move(lock_turn)).detach();
            }
        } else if (Diff(root_m) == -2) {
            if (Diff(root_m->right) == 1) {
                std::unique_ptr<AvlNode> new_root = std::move(root_m->right->left);
                root_m->right->left = std::move(new_root->right);
                new_root->right = std::move(root_m->right);
                root_m->right = std::move(new_root->left);
                new_root->left = std::move(root_m);
                UpdH(new_root->right);
                UpdH(new_root->left);
                UpdH(new_root);
                root = std::move(new_root);
                std::thread(HoldMutex, std::move(lock_turn)).detach();
            } else {
                std::unique_ptr<AvlNode> new_root = std::move(root_m->right);
                root_m->right = std::move(new_root->left);
                new_root->left = std::move(root_m);
                UpdH(new_root->left);
                UpdH(new_root->right);
                UpdH(new_root);
                root = std::move(new_root);
            }
        } else {
            root = std::move(root_m);
        }
    }

    template<class T>
    void Insert(std::unique_ptr<AvlNode> &root, T &&v) {
        if (root == nullptr) {
            std::unique_lock lock_turn(turn_mutex_);
            std::lock_guard lock_draw(drawing_mutex_);
            root = std::make_unique<AvlNode>(std::forward<T>(v));
            UpdH(root);
            std::thread(HoldMutex, std::move(lock_turn)).detach();
            return;
        }
        if (Cmp()(v, root->val)) {
            Insert(root->left, std::forward<T>(v));
        } else if (Cmp { } (root->val, v)) {
            Insert(root->right, std::forward<T>(v));
        }
        UpdH(root);
        Rebalance(root);
    }

    void Erase(std::unique_ptr<AvlNode> &root, Val val) {
        if (root == nullptr) {
            return;
        } else if (Cmp { } (root->val, val)) {
            Erase(root->right, val);
            UpdH(root);
            Rebalance(root);
        } else if (Cmp { } (val, root->val)) {
            Erase(root->left, val);
            UpdH(root);
            Rebalance(root);
        } else if (root->left != nullptr && root->right != nullptr) {
            {
                std::unique_lock lock_turn(turn_mutex_);
                std::lock_guard lock_draw(drawing_mutex_);
                root->val = Begin(root->right)->val;
                std::thread(HoldMutex, std::move(lock_turn)).detach();
            }
            Erase(root->right, root->val);
            UpdH(root);
            Rebalance(root);
        } else if (root->left != nullptr) {
            std::unique_lock lock_turn(turn_mutex_);
            std::lock_guard lock_draw(drawing_mutex_);
            root = std::move(root->left);
            std::thread(HoldMutex, std::move(lock_turn)).detach();
        } else {
            std::unique_lock lock_turn(turn_mutex_);
            std::lock_guard lock_draw(drawing_mutex_);
            root = std::move(root->right);
            std::thread(HoldMutex, std::move(lock_turn)).detach();
        }
    }
};

