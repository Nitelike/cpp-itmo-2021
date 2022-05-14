#pragma once

#include <utility>
#include <vector>

struct Node
{
    int value;
    double key;
    Node *left, *right;

    void delete_this();

    Node(int value);
    ~Node();
};

class Treap
{
    Node * root = nullptr;
    std::size_t m_size = 0;

    std::pair<Node *, Node *> split(Node * t, int x);
    Node * merge(Node * left, Node * right);

    Node * remove_impl(Node * t, int value);
    Node * insert_impl(Node * t, Node * new_node);

public:
    bool contains(int value) const;
    bool insert(int value);
    bool remove(int value);

    std::size_t size() const;
    bool empty() const;

    std::vector<int> values() const;

    ~Treap();
};
