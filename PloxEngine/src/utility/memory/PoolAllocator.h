//
// Created by Ploxie on 2023-05-19.
//

#pragma once
#include "IAllocator.h"

class PoolAllocator : public IAllocator
{
public:
    explicit PoolAllocator(char* memory, size_t elementSize, size_t elementCount, const char* name = nullptr) noexcept;
    explicit PoolAllocator(size_t elementSize, size_t elementCount, const char* name = nullptr) noexcept;
    PoolAllocator(PoolAllocator&&) noexcept;
    ~PoolAllocator();

    PoolAllocator(const PoolAllocator&) noexcept	    = delete;
    PoolAllocator& operator=(const PoolAllocator&) noexcept = delete;
    PoolAllocator& operator=(PoolAllocator&&) noexcept	    = delete;

    void* allocate(size_t n, int flags = 0) noexcept override;
    void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) noexcept override;
    void deallocate(void* p, size_t n) noexcept override;
    const char* get_name() const noexcept override;
    void set_name(const char* pName) noexcept override;

    size_t GetFreeElementCount() const noexcept;

private:
    const char* m_name		 = nullptr;
    const size_t m_elementSize	 = 0;
    const size_t m_elementCount	 = 0;
    char* m_memory		 = nullptr;
    size_t m_freeElementCount	 = 0;
    uint32_t m_freeListHeadIndex = 0xFFFFFFFF;
    bool m_ownsMemory		 = false;
};

class DynamicPoolAllocator : public IAllocator
{
public:
    explicit DynamicPoolAllocator(size_t elementSize, size_t initialElementCount, const char* name = nullptr) noexcept;
    DynamicPoolAllocator(DynamicPoolAllocator&&) noexcept;
    ~DynamicPoolAllocator() override;

    DynamicPoolAllocator(const DynamicPoolAllocator&) noexcept		  = delete;
    DynamicPoolAllocator& operator=(const DynamicPoolAllocator&) noexcept = delete;
    DynamicPoolAllocator& operator=(DynamicPoolAllocator&&) noexcept	  = delete;

    void* allocate(size_t n, int flags = 0) noexcept override;
    void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) noexcept override;
    void deallocate(void* p, size_t n) noexcept override;
    const char* get_name() const noexcept override;
    void set_name(const char* pName) noexcept override;

    size_t GetFreeElementCount() const noexcept;
    void ClearEmptyPools() noexcept;

private:
    struct Pool
    {
	Pool* m_nextPool	     = nullptr;
	char* m_memory		     = nullptr;
	size_t m_elementCount	     = 0;
	size_t m_freeElementCount    = 0;
	uint32_t m_freeListHeadIndex = 0xFFFFFFFF;
    };

    const char* m_name;
    size_t m_elementSize;
    size_t m_freeElementCount = 0;
    size_t m_nextPoolCapacity = 0;
    Pool* m_pools	      = nullptr;
};