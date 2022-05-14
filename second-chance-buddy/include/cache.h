#pragma once

#include "allocator.h"

#include <algorithm>
#include <cstddef>
#include <list>
#include <new>
#include <ostream>
#include <utility>

template <class Key, class KeyProvider, class Allocator>
class Cache
{
public:
    template <class... AllocArgs>
    Cache(const std::size_t cache_size, AllocArgs &&... alloc_args)
        : m_max_size(cache_size)
        , m_alloc(std::forward<AllocArgs>(alloc_args)...)
    {
    }

    std::size_t size() const
    {
        return m_queue.size();
    }

    bool empty() const
    {
        return size() == 0;
    }

    template <class T>
    T & get(const Key & key);

    std::ostream & print(std::ostream & strm) const;

    friend std::ostream & operator<<(std::ostream & strm, const Cache & cache)
    {
        return cache.print(strm);
    }

private:
    const std::size_t m_max_size;
    Allocator m_alloc;
    std::list<std::pair<KeyProvider *, bool>> m_queue;
};

template <class Key, class KeyProvider, class Allocator>
template <class T>
inline T & Cache<Key, KeyProvider, Allocator>::get(const Key & key)
{
    auto it = std::find_if(m_queue.begin(), m_queue.end(), [&key](const auto p) {
        return *(p.first) == key;
    });
    if (it != m_queue.end()) {
        it->second = true;
        return static_cast<T &>(*(it->first));
    }
    while (m_max_size == m_queue.size()) {
        if (m_queue.back().second) {
            m_queue.splice(m_queue.begin(), m_queue, --m_queue.end());
            m_queue.front().second = false;
        }
        else {
            m_alloc.template destroy<KeyProvider>(m_queue.back().first);
            m_queue.pop_back();
        }
    }
    m_queue.push_front({m_alloc.template create<T>(key), false});
    return static_cast<T &>(*(m_queue.front().first));
}

template <class Key, class KeyProvider, class Allocator>
inline std::ostream & Cache<Key, KeyProvider, Allocator>::print(std::ostream & strm) const
{
    bool first = true;
    for (const auto ptr : m_queue) {
        if (!first) {
            strm << " ";
        }
        else {
            first = false;
        }
        strm << *(ptr.first);
    }
    return strm << (first ? "<empty>" : "") << "\n";
}
