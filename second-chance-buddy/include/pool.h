#pragma once

#include <cstddef>
#include <list>
#include <new>
#include <vector>

class PoolAllocator
{
public:
    PoolAllocator(const unsigned min_p, const unsigned max_p);

    void * allocate(const std::size_t n);

    void deallocate(const void * ptr);

private:
    int m_min_p, m_max_p;
    std::vector<std::list<int>> m_free_list;
    std::vector<std::byte> m_storage;
    std::vector<int> m_used_map;

    int len_by_ind(const int n) const;

    int lvl_by_ind(const int n) const;

    int lbord_by_ind(const int n) const;
};
