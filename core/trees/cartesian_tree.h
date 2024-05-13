#pragma once

#include "../physics/tree_drawer.h"
#include "bits/stdc++.h"

struct CtNode;

typedef std::unique_ptr<CtNode> CartezianTree;

int64_t Rand();

struct CtNode : public BaseNode {
    explicit CtNode(int64_t v);

    const BaseNode *GetLeft() const override;

    const BaseNode *GetRight() const override;

    std::string GetInfo() const override;

    int64_t p = Rand();
    size_t size = 1;
    CartezianTree l, r;
};

void Upd(const CartezianTree &n);

size_t GetSize(const CartezianTree &tr);


std::pair<CartezianTree, CartezianTree> SplitBy(CartezianTree tr, int64_t val);

std::pair<CartezianTree, CartezianTree> SplitK(CartezianTree tr, int64_t k);

CartezianTree Merge(CartezianTree l, CartezianTree r);

class CartesianSet : public TreeInterface<> {
public:
    void Insert(const int64_t &val) override;
    void Erase(const int64_t &val) override;

    void lock() override {
        drawing_mutex_.lock();
    }

    void unlock() override {
        drawing_mutex_.unlock();
    }

    const BaseNode *GetRoot() const {
        return root_.get();
    }

private:
    CartezianTree root_;
    std::mutex drawing_mutex_;
};
