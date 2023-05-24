//
// Created by Ploxie on 2023-05-22.
//
#include "VulkanBuffer.h"
#include "volk.h"
#include "VulkanGraphicsAdapter.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanUtilities.h"

VulkanBuffer::VulkanBuffer(VkBuffer buffer, void* allocHandle, const BufferCreateInfo& createInfo, VulkanMemoryAllocator* allocator, VulkanGraphicsAdapter* adapter)
    : m_buffer(buffer), m_allocHandle(allocHandle), m_description(createInfo), m_allocator(allocator), m_adapter(adapter)
{
}

void* VulkanBuffer::GetNativeHandle() const
{
    return m_buffer;
}

const BufferCreateInfo& VulkanBuffer::GetDescription() const
{
    return m_description;
}

void VulkanBuffer::Map(void** data)
{
    m_allocator->MapMemory(static_cast<VulkanAllocationHandle>(m_allocHandle), data);
}

void VulkanBuffer::Unmap()
{
    m_allocator->UnmapMemory(static_cast<VulkanAllocationHandle>(m_allocHandle));
}

void VulkanBuffer::Invalidate(uint32_t count, const MemoryRange* ranges)
{
    const auto allocationInfo	       = m_allocator->GetAllocationInfo(static_cast<VulkanAllocationHandle>(m_allocHandle));
    const uint64_t nonCoherentAtomSize = m_adapter->GetDeviceProperties().limits.nonCoherentAtomSize;

    constexpr uint32_t BATCH_SIZE = 16;
    const uint32_t iterations	  = (count + (BATCH_SIZE - 1)) / BATCH_SIZE;

    for(uint32_t i = 0; i < iterations; i++)
    {
	uint32_t countVk = MIN(BATCH_SIZE, count - i * BATCH_SIZE);
	VkMappedMemoryRange rangesVk[BATCH_SIZE];
	for(uint32_t j = 0; j < countVk; j++)
	{
	    const auto& range = ranges[i * BATCH_SIZE + j];
	    rangesVk[j]	      = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
	    {
		rangesVk[j].memory = allocationInfo.Memory;
		rangesVk[j].offset = Util::AlignDown(allocationInfo.Offset + range.m_offset, nonCoherentAtomSize);
		rangesVk[j].size   = Util::AlignUp(range.m_size, nonCoherentAtomSize);
	    }
	    VulkanUtilities::checkResult(vkInvalidateMappedMemoryRanges(m_adapter->GetDevice(), countVk, rangesVk));
	}
    }
}

void VulkanBuffer::Flush(uint32_t count, const MemoryRange* ranges)
{
    const auto allocationInfo	       = m_allocator->GetAllocationInfo(static_cast<VulkanAllocationHandle>(m_allocHandle));
    const uint64_t nonCoherentAtomSize = m_adapter->GetDeviceProperties().limits.nonCoherentAtomSize;

    constexpr uint32_t BATCH_SIZE = 16;
    const uint32_t iterations	  = (count + (BATCH_SIZE - 1)) / BATCH_SIZE;
    for(uint32_t i = 0; i < iterations; ++i)
    {
	uint32_t countVk = eastl::min(BATCH_SIZE, count - i * BATCH_SIZE);
	VkMappedMemoryRange rangesVk[BATCH_SIZE];
	for(uint32_t j = 0; j < countVk; ++j)
	{
	    const auto& range = ranges[i * BATCH_SIZE + j];
	    rangesVk[j]	      = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
	    {
		rangesVk[j].memory = allocationInfo.Memory;
		rangesVk[j].offset = Util::AlignDown(allocationInfo.Offset + range.m_offset, nonCoherentAtomSize);
		rangesVk[j].size   = Util::AlignUp(range.m_size, nonCoherentAtomSize);
	    }
	}
	VulkanUtilities::checkResult(vkFlushMappedMemoryRanges(m_adapter->GetDevice(), countVk, rangesVk));
    }
}

void* VulkanBuffer::GetAllocationHandle()
{
    return m_allocHandle;
}

VkDeviceMemory VulkanBuffer::GetMemory() const
{
    return m_allocator->GetAllocationInfo(static_cast<VulkanAllocationHandle>(m_allocHandle)).Memory;
}

VkDeviceSize VulkanBuffer::GetOffset() const
{
    return m_allocator->GetAllocationInfo(static_cast<VulkanAllocationHandle>(m_allocHandle)).Offset;
}

VulkanBufferView::VulkanBufferView(VkDevice device, const BufferViewCreateInfo& createInfo)
    : m_device(device), m_bufferView(VK_NULL_HANDLE), m_description(createInfo)
{
    const auto* buffer = dynamic_cast<const VulkanBuffer*>(createInfo.Buffer);
    ASSERT(buffer);

    VkBufferViewCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO };
    {
	bufferCreateInfo.buffer = (VkBuffer) buffer->GetNativeHandle();
	bufferCreateInfo.format = VulkanUtilities::Translate(createInfo.Format);
	bufferCreateInfo.offset = createInfo.Offset;
	bufferCreateInfo.range	= createInfo.Range;
    }

    VulkanUtilities::checkResult(vkCreateBufferView(m_device, &bufferCreateInfo, nullptr, &m_bufferView));
}

VulkanBufferView::~VulkanBufferView()
{
    vkDestroyBufferView(m_device, m_bufferView, nullptr);
}

void* VulkanBufferView::GetNativeHandle() const
{
    return m_bufferView;
}

const Buffer* VulkanBufferView::GetBuffer() const
{
    return m_description.Buffer;
}

const BufferViewCreateInfo& VulkanBufferView::GetDescription() const
{
    return m_description;
}
