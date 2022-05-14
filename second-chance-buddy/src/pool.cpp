#include "pool.h"

#include <algorithm>

namespace {

int ceil_log(const int n)
{
    int i = 0;
    while ((1 << i) < n) {
        i++;
    }
    return i;
}

int floor_log(const int n)
{
    return ceil_log(n) - static_cast<int>((n & (n - 1)) > 0);
}

} // anonymous namespace

int PoolAllocator::len_by_ind(const int n) const
{
    return (1 << (m_max_p - floor_log(n)));
}

int PoolAllocator::lvl_by_ind(const int n) const
{
    return m_max_p - floor_log(n);
}

int PoolAllocator::lbord_by_ind(const int n) const
{
    return len_by_ind(n) * (n - (1 << floor_log(n)));
}

PoolAllocator::PoolAllocator(const unsigned min_p, const unsigned max_p)
    : m_min_p(min_p)
    , m_max_p(max_p)
    , m_free_list(max_p + 1)
    , m_storage(1 << max_p)
    , m_used_map(1 << max_p, 0)
{
    m_free_list[m_max_p].push_back(1);
}

void * PoolAllocator::allocate(const std::size_t n)
{
    int pow = std::max(m_min_p, ceil_log(n));
    auto it = std::find_if(m_free_list.begin() + pow, m_free_list.end(), [](const auto v) {
        return !v.empty();
    });
    if (it == m_free_list.end()) {
        throw std::bad_alloc{};
    }
    int free_pow = it - m_free_list.begin();
    while (free_pow > pow) {
        int ind = m_free_list[free_pow].front();
        m_free_list[free_pow].pop_front();
        m_free_list[free_pow - 1].push_back(ind * 2);
        m_free_list[free_pow - 1].push_back(ind * 2 + 1);
        free_pow--;
    }
    int ind = m_free_list[pow].front();
    m_free_list[pow].pop_front();
    m_used_map[lbord_by_ind(ind)] = ind;
    return &m_storage[lbord_by_ind(ind)];
}

void PoolAllocator::deallocate(const void * ptr)
{
    int pos = static_cast<const std::byte *>(ptr) - &m_storage[0];
    int ind = m_used_map[pos];
    int lvl = lvl_by_ind(ind);
    while (lvl <= m_max_p) {
        auto it = find(m_free_list[lvl].begin(), m_free_list[lvl].end(), ind ^ 1);
        if (it != m_free_list[lvl].end()) {
            m_free_list[lvl].erase(it);
        }
        else {
            break;
        }
        lvl++;
        ind >>= 1;
    }
    m_used_map[pos] = 0;
    m_free_list[lvl].push_back(ind);
}