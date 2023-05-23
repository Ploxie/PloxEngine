//
// Created by Ploxie on 2023-05-19.
//

#pragma once
#include "core/Assert.h"
#include <memory>

struct PlacementNewDummy
{
    explicit PlacementNewDummy() = default;
};

// Overrides for new/delete with a dummy parameter to ensure there won't be a conflict if a library user
// tries to override global placement new.
void* operator new(size_t size, void* pObjMem, PlacementNewDummy dummy) noexcept;

// Override for delete should never be called, this is only here to make sure there are no compiler warnings.
void operator delete(void* pObj, void* pObjMem, PlacementNewDummy dummy) noexcept;

#define PLACEMENT_NEW(memory)	     new((memory), PlacementNewDummy {})
#define ALLOC_NEW(allocator, type)   PLACEMENT_NEW((allocator)->allocate(sizeof(type), alignof(type), 0)) type
#define ALLOC_DELETE(allocator, ptr) (allocator)->DeleteObject(ptr)

class IAllocator
{
public:
    virtual ~IAllocator() noexcept							      = default;
    virtual void* allocate(size_t n, int flags = 0) noexcept				      = 0;
    virtual void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) noexcept = 0;
    virtual void deallocate(void* p, size_t n) noexcept					      = 0;
    virtual const char* get_name() const noexcept					      = 0;
    virtual void set_name(const char* pName) noexcept					      = 0;

    template<typename T>
    T* AllocateArray(size_t count) noexcept
    {
	return reinterpret_cast<T*>(allocate(count * sizeof(T), alignof(T), 0));
    }

    template<typename T>
    void DeleteObject(T* object) noexcept
    {
	if(object)
	{
	    object->~T();
	    deallocate(object, sizeof(T));
	}
    }
};

class ScopedAllocation
{
public:
    explicit ScopedAllocation(IAllocator* allocator, void* allocation, size_t allocationSize = 0) noexcept
	: m_allocator(allocator),
	  m_allocation(allocation),
	  m_allocationSize(allocationSize)
    {
	ASSERT(allocation);
    }

    ScopedAllocation(const ScopedAllocation&)		 = delete;
    ScopedAllocation(ScopedAllocation&&)		 = delete;
    ScopedAllocation& operator=(const ScopedAllocation&) = delete;

    ~ScopedAllocation() noexcept
    {
	m_allocator->deallocate(m_allocation, m_allocationSize);
    }

    void* GetAllocation() const noexcept
    {
	return m_allocation;
    }

    size_t GetAllocationSize() const noexcept
    {
	return m_allocationSize;
    }

private:
    IAllocator* m_allocator;
    void* m_allocation;
    size_t m_allocationSize;
};

void* operator new(std::size_t count, IAllocator* allocator) noexcept;
void* operator new[](std::size_t count, IAllocator* allocator) noexcept;
void* operator new(std::size_t count, std::size_t al, IAllocator* allocator) noexcept;
void* operator new[](std::size_t count, std::size_t al, IAllocator* allocator) noexcept;
void operator delete(void* ptr, IAllocator* allocator) noexcept;
void operator delete[](void* ptr, IAllocator* allocator) noexcept;