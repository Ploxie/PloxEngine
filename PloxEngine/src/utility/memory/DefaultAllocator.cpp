//
// Created by Ploxie on 2023-05-21.
//
#include "DefaultAllocator.h"

DefaultAllocator* DefaultAllocator::Get() noexcept
{
    static DefaultAllocator instance;
    return &instance;
}

void* DefaultAllocator::allocate(size_t n, int flags) noexcept
{
    void* ptr = malloc(n);
    return ptr;
}

void* DefaultAllocator::allocate(size_t n, size_t alignment, size_t offset, int flags) noexcept
{
    void* ptr = malloc(n);
    return ptr;
}

void DefaultAllocator::deallocate(void* p, size_t n) noexcept
{
    free(p);
}

const char* DefaultAllocator::get_name() const noexcept
{
    return m_name;
}

void DefaultAllocator::set_name(const char* pName) noexcept
{
    m_name = pName;
}

void* operator new(size_t size, void* pObjMem, PlacementNewDummy dummy) noexcept
{
    ASSERT(pObjMem);
    return pObjMem;
}

void operator delete(void* pObj, void* pObjMem, PlacementNewDummy dummy) noexcept
{
    ASSERT(false);
}

void* operator new(std::size_t count)
{
    return DefaultAllocator::Get()->allocate(count);
}

void* operator new[](std::size_t count)
{
    return DefaultAllocator::Get()->allocate(count);
}

void* operator new(std::size_t count, const std::nothrow_t&) noexcept
{
    return DefaultAllocator::Get()->allocate(count);
}

void* operator new[](std::size_t count, const std::nothrow_t&) noexcept
{
    return DefaultAllocator::Get()->allocate(count);
}

void operator delete(void* ptr) noexcept
{
    DefaultAllocator::Get()->deallocate(ptr, 0);
}

void operator delete[](void* ptr) noexcept
{
    DefaultAllocator::Get()->deallocate(ptr, 0);
}

void operator delete(void* ptr, std::size_t sz) noexcept
{
    DefaultAllocator::Get()->deallocate(ptr, 0);
}

void operator delete[](void* ptr, std::size_t sz) noexcept
{
    DefaultAllocator::Get()->deallocate(ptr, 0);
}

void* operator new(std::size_t count, IAllocator* allocator) noexcept
{
    return allocator->allocate(count);
}

void* operator new[](std::size_t count, IAllocator* allocator) noexcept
{
    return allocator->allocate(count);
}

void* operator new(std::size_t count, std::size_t al, IAllocator* allocator) noexcept
{
    return allocator->allocate(count, al, 0);
}

void* operator new[](std::size_t count, std::size_t al, IAllocator* allocator) noexcept
{
    return allocator->allocate(count, al, 0);
}

void operator delete(void* ptr, IAllocator* allocator) noexcept
{
    allocator->deallocate(ptr, 0);
}

void operator delete[](void* ptr, IAllocator* allocator) noexcept
{
    allocator->deallocate(ptr, 0);
}
