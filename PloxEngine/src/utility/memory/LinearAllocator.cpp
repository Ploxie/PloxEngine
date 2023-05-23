//
// Created by Ploxie on 2023-05-19.
//

#include "LinearAllocator.h"
#include "utility/Utilities.h"

LinearAllocator::LinearAllocator(char* memory, size_t stackSizeBytes, const char* name) noexcept
    : m_name(name), m_stackSizeBytes(stackSizeBytes), m_memory(memory), m_ownsMemory(false)
{
}
LinearAllocator::LinearAllocator(size_t stackSizeBytes, const char* name) noexcept
    : m_name(name), m_stackSizeBytes(stackSizeBytes), m_memory(static_cast<char*>(malloc(stackSizeBytes))), m_ownsMemory(true)
{
}
LinearAllocator::~LinearAllocator()
{
    if(m_ownsMemory)
    {
	free(m_memory);
    }
}
LinearAllocator::LinearAllocator(LinearAllocator&& other) noexcept
    : m_name(other.m_name), m_stackSizeBytes(other.m_stackSizeBytes), m_memory(other.m_memory), m_currentOffset(other.m_currentOffset), m_ownsMemory(other.m_ownsMemory)
{
    other.m_memory	  = nullptr;
    other.m_currentOffset = 0;
    other.m_ownsMemory	  = false;
}
void* LinearAllocator::allocate(size_t n, int flags) noexcept
{
    size_t newOffset = m_currentOffset + n;
    if(newOffset <= m_stackSizeBytes)
    {
	char* resultPtr = m_memory + m_currentOffset;
	m_currentOffset = newOffset;

	return resultPtr;
    }
    return nullptr;
}
void* LinearAllocator::allocate(size_t n, size_t alignment, size_t offset, int flags) noexcept
{
    size_t curAlignedOffset = Util::AlignPow2Up(m_currentOffset, alignment);
    size_t newOffset	    = curAlignedOffset + n;

    if(newOffset <= m_stackSizeBytes)
    {
	char* resultPtr = m_memory + curAlignedOffset + offset;
	m_currentOffset = newOffset;

	return resultPtr;
    }
    return nullptr;
}
void LinearAllocator::deallocate(void* p, size_t n) noexcept
{
}
const char* LinearAllocator::get_name() const noexcept
{
    return m_name;
}
void LinearAllocator::set_name(const char* pName) noexcept
{
    m_name = pName;
}
LinearAllocator::Marker LinearAllocator::GetMarker() noexcept
{
    return m_currentOffset;
}
void LinearAllocator::FreeToMarker(LinearAllocator::Marker marker) noexcept
{
    ASSERT(marker <= m_currentOffset);
    m_currentOffset = marker;
}
void LinearAllocator::Reset() noexcept
{
    m_currentOffset = 0;
}

LinearAllocatorFrame::LinearAllocatorFrame(LinearAllocator* allocator, const char* name) noexcept
    : m_allocator(allocator),
      m_name(name),
      m_startMarker(allocator->GetMarker())
{
}

LinearAllocatorFrame::~LinearAllocatorFrame() noexcept
{
    m_allocator->FreeToMarker(m_startMarker);
}

void* LinearAllocatorFrame::allocate(size_t n, int flags) noexcept
{
    return m_allocator->allocate(n, flags);
}

void* LinearAllocatorFrame::allocate(size_t n, size_t alignment, size_t offset, int flags) noexcept
{
    return m_allocator->allocate(n, alignment, offset, flags);
}

void LinearAllocatorFrame::deallocate(void* p, size_t n) noexcept
{
    m_allocator->deallocate(p, n);
}

const char* LinearAllocatorFrame::get_name() const noexcept
{
    return m_name;
}

void LinearAllocatorFrame::set_name(const char* pName) noexcept
{
    m_name = pName;
}
