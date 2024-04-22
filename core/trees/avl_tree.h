#include "../physics/tree_drawer.h"

#include <functional>

using Val = int64_t;


template<class Cmp = std::less<>>
class AvlSet {
public:
    struct AvlNode : public TreesDrawer::Node {
        AvlNode(int64_t val)
            : TreesDrawer::Node(val)
        {}

        const TreesDrawer::Node *GetLeft() const override {
            return left.get();
        }
        const TreesDrawer::Node *GetRight() const override {
            return right.get();
        }

        void ProcessEvent(sf::Event event) override {

        }

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override {

        }

        int height = 1;
        std::unique_ptr<AvlNode> left, right;
    };

    template<class T>
    void Insert(T &&v) {
        std::lock_guard lock(operation_);
        Insert(root_, std::forward<T>(v));
    }

    void Erase(Val v) {
        std::lock_guard lock(operation_);
        Erase(root_, v);
    }

    const std::unique_ptr<AvlNode> &GetRoot() const {
        return root_;
    }

    void SetCallBacks(std::function<void()> &&cb_beg, std::function<void(const AvlNode*)> &&cb_end) {
        cb_begin_transaction_ = std::move(cb_beg);
        cb_end_transaction_ = std::move(cb_end);
    }

private:
    std::function<void()> cb_begin_transaction_ = [] () {};
    std::function<void(const AvlNode*)> cb_end_transaction_ = [] (const AvlNode*) {};
    std::unique_ptr<AvlNode> root_;
    std::mutex operation_;

    template<class T>
    void Insert(std::unique_ptr<AvlNode> &root, T &&v) {
        if (root == nullptr) {
            cb_begin_transaction_();
            root = std::make_unique<AvlNode>(std::forward<T>(v));
            cb_end_transaction_(root_.get());
            UpdH(root);
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

    static std::unique_ptr<AvlNode> Begin(std::unique_ptr<AvlNode> root) {
        return root ? root->l ? Begin(root->l) : root : nullptr;
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
            cb_begin_transaction_();
            root->val = Begin(root->right)->val;
            cb_end_transaction_(root_.get());
            Erase(root->right, root->val);
            UpdH(root);
            Rebalance(root);
        } else if (root->left != nullptr) {
            cb_begin_transaction_();
            root = root->left;
            cb_end_transaction_(root_.get());
        } else {
            cb_begin_transaction_();
            root = root->right;
            cb_end_transaction_(root_.get());
        }
    }

    static int GetH(const std::unique_ptr<AvlNode> &v) {
        return v ? v->height : 0;
    }

    static int Diff(const std::unique_ptr<AvlNode> &v) {
        return v ? GetH(v->left) - GetH(v->right) : 0;
    }

    void Rebalance(std::unique_ptr<AvlNode> &root) {
        std::unique_ptr<AvlNode> root_m = std::move(root);
        if (Diff(root_m) == 2) {
            if (Diff(root_m->left) == -1) {
                cb_begin_transaction_();
                std::unique_ptr<AvlNode> new_root = std::move(root_m->left->right);
                root_m->left->right = std::move(new_root->left);
                new_root->left = std::move(root_m->left);
                root_m->left = std::move(new_root->right);
                new_root->right = std::move(root_m);
                UpdH(new_root->left);
                UpdH(new_root->right);
                UpdH(new_root);
                root = std::move(new_root);
                cb_end_transaction_(root_.get());
            } else {
                cb_begin_transaction_();
                std::unique_ptr<AvlNode> new_root = std::move(root_m->left);
                root_m->left = std::move(new_root->right);
                new_root->right = std::move(root_m);
                UpdH(new_root->left);
                UpdH(new_root->right);
                UpdH(new_root);
                root = std::move(new_root);
                cb_end_transaction_(root_.get());
            }
        } else if (Diff(root_m) == -2) {
            if (Diff(root_m->right) == 1) {
                cb_begin_transaction_();
                std::unique_ptr<AvlNode> new_root = std::move(root_m->right->left);
                root_m->right->left = std::move(new_root->right);
                new_root->right = std::move(root_m->right);
                root_m->right = std::move(new_root->left);
                new_root->left = std::move(root_m);
                UpdH(new_root->right);
                UpdH(new_root->left);
                UpdH(new_root);
                root = std::move(new_root);
                cb_end_transaction_(root_.get());
            } else {
                cb_begin_transaction_();
                std::unique_ptr<AvlNode> new_root = std::move(root_m->right);
                root_m->right = std::move(new_root->left);
                new_root->left = std::move(root_m);
                UpdH(new_root->left);
                UpdH(new_root->right);
                UpdH(new_root);
                root = std::move(new_root);
                cb_end_transaction_(root_.get());
            }
        } else {
            root = std::move(root_m);
        }
    }
};

