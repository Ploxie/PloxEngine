//
// Created by Ploxie on 2023-05-19.
//

#pragma once
#include "IAllocator.h"

class LinearAllocator : public IAllocator
{
public:
    using Marker = size_t;

    explicit LinearAllocator(char* memory, size_t stackSizeBytes, const char* name = nullptr) noexcept;
    explicit LinearAllocator(size_t stackSizeBytes, const char* name = nullptr) noexcept;
    ~LinearAllocator() override;

    LinearAllocator(LinearAllocator&&) noexcept;
    LinearAllocator(const LinearAllocator&) noexcept = delete;

    LinearAllocator& operator=(const LinearAllocator&) noexcept = delete;
    LinearAllocator& operator=(LinearAllocator&&) noexcept	= delete;

    void* allocate(size_t n, int flags = 0) noexcept override;
    void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) noexcept override;
    void deallocate(void* p, size_t n) noexcept override;
    const char* get_name() const noexcept override;
    void set_name(const char* pName) noexcept override;

    Marker GetMarker() noexcept;
    void FreeToMarker(Marker marker) noexcept;
    void Reset() noexcept;

protected:
    const char* m_name		  = nullptr;
    const size_t m_stackSizeBytes = 0;
    char* m_memory		  = nullptr;
    size_t m_currentOffset	  = 0;
    bool m_ownsMemory		  = false;
};

class LinearAllocatorFrame : public IAllocator
{
public:
    explicit LinearAllocatorFrame(LinearAllocator* allocator, const char* name = nullptr) noexcept;
    ~LinearAllocatorFrame() noexcept;

    LinearAllocatorFrame(const LinearAllocatorFrame&)		 = delete;
    LinearAllocatorFrame(LinearAllocatorFrame&&)		 = delete;
    LinearAllocatorFrame& operator=(const LinearAllocatorFrame&) = delete;

    void* allocate(size_t n, int flags = 0) noexcept override;
    void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) noexcept override;
    void deallocate(void* p, size_t n) noexcept override;
    const char* get_name() const noexcept override;
    void set_name(const char* pName) noexcept override;

private:
    LinearAllocator* m_allocator;
    const char* m_name;
    LinearAllocator::Marker m_startMarker;
};

template<typename T, size_t SIZE>
class StackLinearAllocator : public LinearAllocator
{
public:
    explicit StackLinearAllocator(const char* name = nullptr) noexcept
	: LinearAllocator(static_cast<char*>(m_data), sizeof(T) * SIZE, name)
    {
    }

private:
    T m_data[SIZE];
};
