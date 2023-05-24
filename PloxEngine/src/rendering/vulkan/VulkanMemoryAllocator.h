//
// Created by Ploxie on 2023-05-21.
//

#pragma once
#include "EASTL/vector.h"
#include "utility/memory/TLSFAllocator.h"
#include "vulkan/vulkan.h"

using VulkanAllocationHandle = struct AllocationHandleVk_t*;

struct VulkanAllocationCreateInfo
{
    VkMemoryPropertyFlags RequiredFlags;
    VkMemoryPropertyFlags PreferredFlags;
    bool DedicatedAllocation;
};

struct VulkanAllocationInfo
{
    VkDeviceMemory Memory;
    VkDeviceSize Offset;
    VkDeviceSize Size;
    uint32_t MemoryType;
    size_t PoolIndex;
    size_t BlockIndex;
    size_t MapCount;
    void* PoolData;
};

struct VulkanMemoryBlockDebugInfo
{
    uint32_t MemoryType;
    size_t AllocationSize;
    //eastl::vector<TLSFSpanDebugInfo> m_spans;
};

class VulkanMemoryPool
{
public:
    void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t memoryType, uint32_t heapIndex, VkDeviceSize bufferImageGranularity, VkDeviceSize preferredBlockSize, VkDeviceSize* heapUsage, VkDeviceSize heapSizeLimit, bool useMemoryBudgetExtension);

    VkResult Allocate(VkDeviceSize size, VkDeviceSize alignment, VulkanAllocationInfo& allocationInfo);
    void Free(VulkanAllocationInfo allocationInfo);

    VkResult MapMemory(size_t blockIndex, VkDeviceSize offset, void** data);
    void UnmapMemory(size_t blockIndex);
    /*void Free(VulkanAllocationInfo& allocationInfo);
    VkResult MapMemory(size_t blockIndex, VkDeviceSize offset, void** data);
    void UnmapMemory(size_t blockIndex);
    void GetFreeUsedWastedSizes(size_t& free, size_t& used, size_t wasted) const;
    void Destroy();*/

private:
    void GetBudget(VkDeviceSize& budget, VkDeviceSize& usage);
    bool AllocateFromBlock(size_t blockIndex, VkDeviceSize size, VkDeviceSize alignment, VulkanAllocationInfo& allocationInfo);

private:
    enum
    {
	MAX_BLOCKS = 16
    };

    VkDevice m_device			    = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice	    = VK_NULL_HANDLE;
    uint32_t m_memoryType		    = -1;
    uint32_t m_heapIndex		    = -1;
    VkDeviceSize m_bufferImageGranularity   = 0;
    VkDeviceSize m_preferredBlockSize	    = 0;
    VkDeviceSize m_heapSizeLimit	    = 0;
    VkDeviceSize* m_heapUsage		    = nullptr;
    VkDeviceSize m_blockSizes[MAX_BLOCKS]   = {};
    VkDeviceMemory m_memory[MAX_BLOCKS]	    = {};
    void* m_mappedPtr[MAX_BLOCKS]	    = {};
    size_t m_mapCount[MAX_BLOCKS]	    = {};
    TLSFAllocator* m_allocators[MAX_BLOCKS] = {};
    alignas(TLSFAllocator) char m_allocatorMemory[MAX_BLOCKS * sizeof(TLSFAllocator)];
    bool m_useMemoryBudgetExtension = false;
};

class VulkanMemoryAllocator
{
public:
    VulkanMemoryAllocator();

    void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, bool useMemBudgetExt);

    VkResult Allocate(const VulkanAllocationCreateInfo& allocationCreateInfo, const VkMemoryRequirements& memoryRequirements, const VkMemoryDedicatedAllocateInfo* dedicatedAllocateInfo, VulkanAllocationHandle& allocationHandle);
    void Free(VulkanAllocationHandle allocationHandle);

    VkResult CreateImage(const VulkanAllocationCreateInfo& allocationCreateInfo, const VkImageCreateInfo& imageCreateInfo, VkImage& image, VulkanAllocationHandle& allocationHandle);
    VkResult CreateBuffer(const VulkanAllocationCreateInfo& allocationCreateInfo, const VkBufferCreateInfo& bufferCreateInfo, VkBuffer& buffer, VulkanAllocationHandle& allocationHandle);

    void DestroyImage(VkImage image, VulkanAllocationHandle allocationHandle);
    void DestroyBuffer(VkBuffer buffer, VulkanAllocationHandle allocationHandle);

    VkResult MapMemory(VulkanAllocationHandle allocationHandle, void** data);
    void UnmapMemory(VulkanAllocationHandle allocationHandle);

    VulkanAllocationInfo GetAllocationInfo(VulkanAllocationHandle allocationHandle);

private:
    VkResult FindMemoryTypeIndex(uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties, VkMemoryPropertyFlags preferredProperties, uint32_t& memoryTypeIndex);

private:
    enum
    {
	MAX_BLOCK_SIZE = 256 * 1024 * 1024
    };

    VkDevice m_device					= VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice			= VK_NULL_HANDLE;
    VkPhysicalDeviceMemoryProperties m_memoryProperties = {};
    VkDeviceSize m_bufferImageGranularity		= 0;
    VkDeviceSize m_nonCoherentAtomSize			= 0;
    VkDeviceSize m_heapSizeLimits[VK_MAX_MEMORY_HEAPS]	= {};
    VkDeviceSize m_heapUsage[VK_MAX_MEMORY_HEAPS]	= {};
    VulkanMemoryPool m_pools[VK_MAX_MEMORY_TYPES]	= {};
    DynamicObjectPool<VulkanAllocationInfo> m_allocationInfoPool;
    bool m_useMemoryBudgetExtension = false;
};
