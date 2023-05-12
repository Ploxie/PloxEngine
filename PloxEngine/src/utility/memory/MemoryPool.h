//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "utility/Utilities.h"

template<typename T>
struct RawData
{
    alignas(alignof(T)) char m_data[sizeof(T)];
};

template<typename T, size_t Count>
class StaticObjectPool
{
public:
    using ObjectType = T;

    explicit StaticObjectPool();
    T* Alloc();
    void Free(T* value);
    size_t GetAllocationCount() const;

private:
    union Item
    {
	size_t m_nextFreeItem;
	T m_value;
    };

    Item m_items[Count];
    size_t m_firstFreeIndex {};
    size_t m_allocationCount {};
};

template<typename T, size_t Count>
using StaticMemoryPool = StaticObjectPool<RawData<T>, Count>;

template<typename T, size_t Count>
inline StaticObjectPool<T, Count>::StaticObjectPool()
    : m_firstFreeIndex(), m_allocationCount()
{
    for(size_t i = 0; i < Count; ++i)
    {
	m_items[i].m_nextFreeItem = i + 1;
    }

    m_items[Count - 1].m_nextFreeItem = ~static_cast<size_t>(0);
}

template<typename T, size_t Count>
inline T* StaticObjectPool<T, Count>::Alloc()
{
    if(m_firstFreeIndex == ~static_cast<size_t>(0))
    {
	return nullptr;
    }

    m_allocationCount++;

    Item& item	     = m_items[m_firstFreeIndex];
    m_firstFreeIndex = item.m_nextFreeItem;
    return &item.m_value;
}

template<typename T, size_t Count>
inline void StaticObjectPool<T, Count>::Free(T* value)
{
    Item* item		 = reinterpret_cast<Item*>(value);
    size_t index	 = item - m_items;
    item->m_nextFreeItem = m_firstFreeIndex;
    m_firstFreeIndex	 = index;
    m_allocationCount--;
}

template<typename T, size_t Count>
inline size_t StaticObjectPool<T, Count>::GetAllocationCount() const
{
    return m_allocationCount;
}