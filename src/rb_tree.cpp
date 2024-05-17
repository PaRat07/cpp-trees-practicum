#include "../core/trees/rb_tree.h"


void RBTree::Ins(Node* v, int64_t val) {
    if (v->val > val) {
        if (v->l == nullptr) {
            v->l = new Node(val);
            v->l->prev = v;
            InsRebalance(v->l);
            return;
        }
        Ins(v->l, val);
    } else {
        if (v->r == nullptr) {
            v->r = new Node(val);
            v->r->prev = v;
            InsRebalance(v->r);
            return;
        }
        Ins(v->r, val);
    }
    InsRebalance(v);
}

void RBTree::HoldMutex(std::unique_lock<std::mutex> lock) {
    std::this_thread::sleep_for(OPERATION_DELAY);
}


void RBTree::Erase(const int64_t &val) {
    Delete(val);
}

RBTree::Node *RBTree::Find(Node* v, int64_t val) {
    if (!v) {
        return nullptr;
    }
    if (v->val == val) {
        return v;
    }
    if (v->val > val) {
        return Find(v->l, val);
    }
    return Find(v->r, val);
}

void RBTree::LeftRotate(Node *a) {
    std::unique_lock lock_turn(turn_mtx_);
    std::lock_guard lock_draw(drawing_mtx_);
    Node *p = a->prev;
    Node *r = a->r;
    if (p) {
        if (p->l == a) {
            p->l = r;
        } else {
            p->r = r;
        }
    }

    auto color = a->r->color;
    a->r->color = a->color;
    a->color = color;

    Node *tmp = r->l;
    r->l = a;
    a->r = tmp;
    a->prev = r;
    r->prev = p;

    if (a->r) {
        a->r->prev = a;
    }
    if (r->prev == nullptr) {
        root_ = r;
    }
    std::thread(HoldMutex, std::move(lock_turn)).detach();
}

void RBTree::RightRotate(Node *a) {
    std::unique_lock lock_turn(turn_mtx_);
    std::lock_guard lock_draw(drawing_mtx_);
    Node* p = a->prev;
    Node* l = a->l;
    if (p) {
        if (p->r == a) {
            p->r = l;
        } else {
            p->l = l;
        }
    }

    auto color = a->l->color;
    a->l->color = a->color;
    a->color = color;

    Node* tmp = l->r;
    l->r = a;
    a->l = tmp;
    a->prev = l;
    l->prev = p;

    if (a->l) {
        a->l->prev = a;
    }
    if (l->prev == nullptr) {
        root_ = l;
    }
    std::thread(HoldMutex, std::move(lock_turn)).detach();
}

void RBTree::InsRebalance(Node *a) {
    if (!a->prev)
        return;
    if (!a->prev->prev || a->color != sf::Color::Red || GetColor(a) != GetColor(a->prev)) {
        return;
    }
    if (a->prev->prev->l == a->prev) {
        if (GetColor(a->prev->prev->r) == sf::Color::Black) {
            if (a->prev->r == a) {
                LeftRotate(a->prev);
                RightRotate(a->prev);
            } else {
                RightRotate(a->prev->prev);
            }
        } else {
            a->prev->prev->r->color = sf::Color::Black;
            a->prev->prev->color = sf::Color::Red;
            a->prev->color = sf::Color::Black;
        }
    } else {
        if (GetColor(a->prev->prev->l) == sf::Color::Black) {
            if (a->prev->l == a) {
                RightRotate(a->prev);
                LeftRotate(a->prev);
            } else {
                LeftRotate(a->prev->prev);
            }
        } else {
            a->prev->prev->l->color = sf::Color::Black;
            a->prev->prev->color = sf::Color::Red;
            a->prev->color = sf::Color::Black;
        }
    }

    if (a->prev == nullptr) {
        root_ = a;
    } else if (a->prev->prev == nullptr) {
        root_ = a->prev;
    }
}

void RBTree::Delete(int64_t val) {
    auto q = Find(root_, val);
    auto replace = GetMin(q->r);
    if (replace == nullptr && q == root_) {
        root_ = (root_->l ? root_->l : nullptr);
        if (root_ != nullptr) {
            root_->prev = nullptr;
            root_->color = sf::Color::Black;
        }
        delete q;
        return;
    }
    if (replace == nullptr && q->l) {
        RightRotate(q);
        q->prev->r = nullptr;
        delete q;
        return;
    }
    if (replace == nullptr) {
        replace = q;
    }

    q->val = replace->val;

    if (replace->color == sf::Color::Red && !replace->l && !replace->r) {
        if (replace->prev->l == replace) {
            replace->prev->l = nullptr;
        } else {
            replace->prev->r = nullptr;
        }
        delete replace;
        return;
    }
    replace->is_empty = true;
    replace->color = sf::Color::Magenta;

    while (replace->color == sf::Color::Magenta) {
        if (replace->prev == nullptr) {
            replace->color = sf::Color::Black;
            root_ = replace;
            break;
        }
        // проверка положения
        if (replace->prev->l == replace) {
            // first
            if (GetColor(replace->prev->r) == sf::Color::Red &&
                GetColor(replace->prev->r->l) == sf::Color::Black &&
                GetColor(replace->prev->r->r) == sf::Color::Black) {
                LeftRotate(replace->prev);
            } else
            // second
            if (GetColor(replace->prev->r) == sf::Color::Black &&
                GetColor(replace->prev->r->l) == sf::Color::Black &&
                GetColor(replace->prev->r->r) == sf::Color::Black) {
                replace->prev->r->color = sf::Color::Red;
                if (replace->prev->color == sf::Color::Red) {
                    replace->prev->color = sf::Color::Black;
                } else {
                    replace->prev->color = sf::Color::Magenta;
                }
                auto e = replace->prev;
                if (replace->is_empty) {
                    replace->prev->l = nullptr;
                    delete replace;
                } else {
                    replace->color = sf::Color::Black;
                }
                replace = e;
            } else
            // third
            if (GetColor(replace->prev->r) == sf::Color::Black &&
                GetColor(replace->prev->r->l) == sf::Color::Red &&
                GetColor(replace->prev->r->r) == sf::Color::Black) {
                RightRotate(replace->prev->r);
            } else
            // fourth
            if (GetColor(replace->prev->r) == sf::Color::Black &&
                GetColor(replace->prev->r->r) == sf::Color::Red) {
                LeftRotate(replace->prev);
                replace->prev->prev->r->color = sf::Color::Black;
                replace->color = sf::Color::Black;
                auto e = replace;
                if (replace->is_empty) {
                    e->prev->l = nullptr;
                    delete replace;
                }
                break;
            }
        } else {
            // first
            if (GetColor(replace->prev->l) == sf::Color::Red &&
                GetColor(replace->prev->l->l) == sf::Color::Black &&
                GetColor(replace->prev->l->r) == sf::Color::Black) {
                RightRotate(replace->prev);
            } else
            // second
            if (GetColor(replace->prev->l) == sf::Color::Black &&
                GetColor(replace->prev->l->l) == sf::Color::Black &&
                GetColor(replace->prev->l->r) == sf::Color::Black) {
                replace->prev->l->color = sf::Color::Red;
                if (replace->prev->color == sf::Color::Red) {
                    replace->prev->color = sf::Color::Black;
                } else {
                    replace->prev->color = sf::Color::Magenta;
                }
                auto e = replace->prev;
                if (replace->is_empty) {
                    replace->prev->r = nullptr;
                    delete replace;
                } else {
                    replace->color = sf::Color::Black;
                }
                replace = e;
            } else
            // third
            if (GetColor(replace->prev->l) == sf::Color::Black &&
                GetColor(replace->prev->l->r) == sf::Color::Red &&
                GetColor(replace->prev->l->l) == sf::Color::Black) {
                LeftRotate(replace->prev->l);
            } else
            // fourth
            if (GetColor(replace->prev->l) == sf::Color::Black &&
                GetColor(replace->prev->l->l) == sf::Color::Red) {
                RightRotate(replace->prev);
                replace->prev->prev->l->color = sf::Color::Black;
                replace->color = sf::Color::Black;
                auto e = replace;
                if (replace->is_empty) {
                    e->prev->r = nullptr;
                    delete replace;
                }
                break;
            }
        }
    }
}

sf::Color RBTree::GetColor(Node *a) {
    if (a == nullptr) {
        return sf::Color::Black;
    }
    return a->color;
}

RBTree::Node *RBTree::GetMin(Node *a) {
    if (!a) {
        return nullptr;
    }
    if (!a->l) {
        if (a->r) {
            LeftRotate(a);
        }
        return a;
    }
    return GetMin(a->l);
}

void RBTree::Insert(const int64_t &val) {
    std::lock_guard lock(operation_mtx_);
    if (Find(root_, val) != nullptr) {
        return;
    }
    if (root_ == nullptr) {
        root_ = new Node(val);
    } else {
        Ins(root_, val);
    }
    if (root_->color == sf::Color::Red) {
        root_->color = sf::Color::Black;
    }
}