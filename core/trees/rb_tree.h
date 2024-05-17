#include <thread>

#include "../physics/tree_drawer.h"


class RBTree final : public TreeInterface<> {
    public:
    struct Node final : public BaseNode {
        Node(int64_t v)
            : BaseNode(v)
        {
            color = sf::Color::Red;
        }


        const BaseNode *GetLeft() const override {
            return l;
        }
        const BaseNode *GetRight() const override {
            return r;
        }
        std::string GetInfo() const override {
            return std::to_string(val);
        }
        Node *l = nullptr, *r = nullptr, *prev = nullptr;
        bool is_empty = false;
    };

    void Insert(const int64_t &val) override;
    void Erase(const int64_t &val) override;

    void lock() {
        drawing_mtx_.lock();
    }
    void unlock() override {
        drawing_mtx_.unlock();
    }

    const BaseNode *GetRoot() const override {
        return root_;
    }

private:
    Node* Find(Node* v, int64_t val);
    void LeftRotate(Node* a);
    void RightRotate(Node* a);
    void InsRebalance(Node* a);
    void Delete(int64_t val);
    sf::Color GetColor(Node* a);
    Node* GetMin(Node* a);
    void Ins(Node* v, int64_t val);
    static void HoldMutex(std::unique_lock<std::mutex> lock);

    Node* root_ = nullptr;
    std::mutex drawing_mtx_;
    std::mutex turn_mtx_;
    std::mutex operation_mtx_;

    static constexpr auto OPERATION_DELAY = std::chrono::milliseconds(1500);
};
