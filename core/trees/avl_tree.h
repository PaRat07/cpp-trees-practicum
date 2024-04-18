#include "../physics/tree_drawer.h"

#include <functional>

template<class Val, class Cmp = std::less<>>
class AvlSet {
public:

    struct AvlNode : public TreesDrawer::Node {
        const std::unique_ptr<Node> &GetLeft() const override {
            return left;
        }
        const std::unique_ptr<Node> &GetRight() const override {
            return right;
        }
        const Node *GetParent() const override {
            return par;
        }

        int height = 1;
        std::unique_ptr<AvlNode> left, right;
        const AvlNode *par;
    };

    void Insert(Val v) {
        root_ = Insert(root_, v);
    }

    void Erase(Val v) {
        root_ = Erase(root_, v);
    }

    const std::unique_ptr<TreesDrawer::Node> &GetRoot() const {
        return root_;
    }

private:
    std::unique_ptr<AvlNode> root_;

    static std::unique_ptr<TreesDrawer::Node> Insert(std::unique_ptr<AvlNode> root, Val v) {
        if (root == nullptr) {
            return nullptr;
        }
        if (Cmp { } (v, root->val)) {
            root->left = Insert(root->l, v);
        } else if (Cmp { } (root->val, v)) {
            root->r = Insert(root->r, v);
        }
        UpdH(root);
        return Rebalance(root);
    }

    static std::unique_ptr<AvlNode> Begin(std::unique_ptr<AvlNode> root) {
        return root ? root->l ? Begin(root->l) : root : nullptr;
    }

    static void UpdH(std::unique_ptr<AvlNode> root) {
        root->height = 1;
        if (root->left) {
            root->height = std::max(root->height, root->left->height + 1);
        }
        if (root->right) {
            root->height = std::max(root->height, root->right->height + 1);
        }
    }

    static std::unique_ptr<AvlNode> Erase(std::unique_ptr<AvlNode> root, Val val) {
        if (root == nullptr) {
            return nullptr;
        } else if (Cmp { } (root->val, val)) {
            root->right = Erase(root->right, val);
            UpdH(root);
            return Rebalance(root);
        } else if (Cmp { } (val, root->val)) {
            root->left = Erase(root->left, val);
            UpdH(root);
            return Rebalance(root);
        } else if (root->left != nullptr && root->right != nullptr) {
            root->val = Begin(root->right)->val;
            root->right = Erase(root->right, root->val);
            UpdH(root);
            return Rebalance(root);
        } else if (root->left != nullptr) {
            std::unique_ptr<AvlNode> ptr = root->left;
            delete root;
            return ptr;
        } else {
            std::unique_ptr<AvlNode> ptr = root->right;
            delete root;
            return ptr;
        }
    }

    static int GetH(std::unique_ptr<AvlNode> v) {
        return v ? v->height : 0;
    }

    static int Diff(std::unique_ptr<AvlNode> v) {
        return v ? GetH(v->left) - GetH(v->right) : 0;
    }

    static std::unique_ptr<AvlNode> Rebalance(std::unique_ptr<AvlNode> root) {
        if (Diff(root) == 2) {
            if (Diff(root->left) == -1) {
                std::unique_ptr<AvlNode> new_root = root->left->right;
                root->left->right = new_root->left;
                new_root->left = root->left;
                root->left = new_root->right;
                new_root->right = root;
                UpdH(new_root->left);
                UpdH(new_root->right);
                UpdH(new_root);
                return new_root;
            } else {
                std::unique_ptr<AvlNode> new_root = root->left;
                root->left = new_root->right;
                new_root->right = root;
                UpdH(new_root->left);
                UpdH(new_root->right);
                UpdH(new_root);
                return new_root;
            }
        } else if (Diff(root) == -2) {
            if (Diff(root->right) == 1) {
                std::unique_ptr<AvlNode> new_root = root->right->left;
                root->right->left = new_root->right;
                new_root->right = root->right;
                root->right = new_root->left;
                new_root->left = root;
                UpdH(new_root->right);
                UpdH(new_root->left);
                UpdH(new_root);
                return new_root;
            } else {
                std::unique_ptr<AvlNode> new_root = root->right;
                root->right = new_root->left;
                new_root->left = root;
                UpdH(new_root->left);
                UpdH(new_root->right);
                UpdH(new_root);
                return new_root;
            }
        } else {
            return root;
        }
    }
};

