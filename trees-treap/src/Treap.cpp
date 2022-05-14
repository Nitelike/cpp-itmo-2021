#include "Treap.h"

#include "RandomGenerator.h"

Node::Node(int value)
    : value(value)
    , key(get_random_number())
    , left(nullptr)
    , right(nullptr)
{
}

void Node::delete_this()
{
    left = right = nullptr;
    delete this;
}

Node::~Node()
{
    delete left;
    delete right;
}

std::pair<Node *, Node *> Treap::split(Node * t, int x)
{
    if (t == nullptr) {
        return {nullptr, nullptr};
    }
    if (x > t->value) {
        auto [t1, t2] = split(t->right, x);
        t->right = t1;
        return {t, t2};
    }
    else {
        auto [t1, t2] = split(t->left, x);
        t->left = t2;
        return {t1, t};
    }
}

Node * Treap::merge(Node * left, Node * right)
{
    if (left == nullptr) {
        return right;
    }
    if (right == nullptr) {
        return left;
    }
    if (left->key > right->key) {
        left->right = merge(left->right, right);
        return left;
    }
    else {
        right->left = merge(left, right->left);
        return right;
    }
}

bool Treap::contains(int value) const
{
    auto ptr = root;
    while (ptr != nullptr) {
        if (ptr->value > value) {
            ptr = ptr->left;
        }
        else if (ptr->value < value) {
            ptr = ptr->right;
        }
        else {
            return true;
        }
    }
    return false;
}

Node * Treap::insert_impl(Node * t, Node * new_node)
{
    if (t == nullptr || new_node->key > t->key) {
        auto [p1, p2] = split(t, new_node->value);
        new_node->left = p1;
        new_node->right = p2;
        return new_node;
    }
    if (new_node->value < t->value) {
        t->left = insert_impl(t->left, new_node);
    }
    else {
        t->right = insert_impl(t->right, new_node);
    }
    return t;
}

bool Treap::insert(int value)
{
    if (contains(value)) {
        return false;
    }
    root = insert_impl(root, new Node(value));
    m_size++;
    return true;
}

Node * Treap::remove_impl(Node * t, int value)
{
    if (t->value == value) {
        auto res = merge(t->left, t->right);
        t->delete_this();
        return res;
    }
    if (value < t->value) {
        t->left = remove_impl(t->left, value);
    }
    else {
        t->right = remove_impl(t->right, value);
    }
    return t;
}

bool Treap::remove(int value)
{
    if (!contains(value)) {
        return false;
    }
    root = remove_impl(root, value);
    m_size--;
    return true;
}

std::size_t Treap::size() const
{
    return m_size;
}

bool Treap::empty() const
{
    return (size() == 0);
}

void dfs(Node * v, std::vector<int> & values)
{
    if (v == nullptr) {
        return;
    }
    dfs(v->left, values);
    values.push_back(v->value);
    dfs(v->right, values);
}

std::vector<int> Treap::values() const
{
    std::vector<int> values;
    values.reserve(size());
    dfs(root, values);
    return values;
}

Treap::~Treap()
{
    delete root;
}