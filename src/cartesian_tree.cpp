#include "../core/trees/cartesian_tree.h"

int64_t Rand() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen();
}

CtNode::CtNode(int64_t v): BaseNode(v) {}

const BaseNode * CtNode::GetLeft() const {
    return l.get();
}

const BaseNode * CtNode::GetRight() const {
    return r.get();
}

std::string CtNode::GetInfo() const {
    return  "Val: " + std::to_string(val) + "\n" +
            "Priority: " + std::to_string(p) + "\n" +
            "Subtree size: " + std::to_string(size);
}

void Upd(const CartezianTree &n) {
    if (n == nullptr) return;
    n->size = (n->l ? n->l->size : 0) + (n->r ? n->r->size : 0) + 1;
}

size_t GetSize(const CartezianTree &tr) {
    return tr == nullptr ? 0 : tr->size;
}

std::pair<CartezianTree, CartezianTree> SplitBy(CartezianTree tr, int64_t val) {
    if (tr == nullptr) {
        return {};
    } else if (tr->val < val) {
        auto [le, nle] = SplitBy(std::move(tr->r), val);
        tr->r = std::move(le);
        Upd(tr);
        Upd(nle);
        return { std::move(tr), std::move(nle) };
    } else {
        auto [le, nle] = SplitBy(std::move(tr->l), val);
        tr->l = std::move(nle);
        Upd(tr);
        Upd(le);
        return {std::move(le), std::move(tr) };
    }
}

std::pair<CartezianTree, CartezianTree> SplitK(CartezianTree tr, int64_t k) {
    if (GetSize(tr) <= k) {
        return { std::move(tr), nullptr };
    } else if (GetSize(tr->l) < k) {
        auto [ngr, gr] = SplitK(std::move(tr->r), k - GetSize(tr->l) - 1);
        tr->r = std::move(ngr);
        Upd(tr);
        Upd(gr);
        return { std::move(tr), std::move(gr) };
    } else {
        auto [ngr, gr] = SplitK(std::move(tr->l), k);
        tr->l = std::move(gr);
        Upd(tr);
        Upd(ngr);
        return {std::move(ngr), std::move(tr)};
    }
}

CartezianTree Merge(CartezianTree l, CartezianTree r) {
    if (l == nullptr) {
        return r;
    } else if (r == nullptr) {
        return l;
    } else if (l->p > r->p) {
        l->r = Merge(std::move(l->r), std::move(r));
        Upd(l);
        return l;
    } else {
        r->l = Merge(std::move(l), std::move(r->l));
        Upd(r);
        return r;
    }
}

void CartesianSet::Insert(const int64_t &val) {
    std::lock_guard lock(drawing_mutex_);
    auto [l, other] = SplitBy(std::move(root_), val);
    auto [need, r] = SplitBy(std::move(other), val + 1);
    need = std::make_unique<CtNode>(val);
    root_ = Merge(std::move(l), Merge(std::move(need), std::move(r)));
}

void CartesianSet::Erase(const int64_t &val) {
    std::lock_guard lock(drawing_mutex_);
    auto [l, other] = SplitBy(std::move(root_), val);
    auto [need, r] = SplitBy(std::move(other), val + 1);
    root_ = Merge(std::move(l), std::move(r));
}
