//
// Created by Ploxie on 2023-05-22.
//

#include "VulkanMemoryAllocator.h"
#include "core/Logger.h"
#include "volk.h"

void VulkanMemoryPool::Initialize(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t memoryType, uint32_t heapIndex, VkDeviceSize bufferImageGranularity, VkDeviceSize prefferedBlockSize, VkDeviceSize* heapUsage, VkDeviceSize heapSizeLimit, bool useMemoryBudgetExtension)
{
    m_device		       = device;
    m_physicalDevice	       = physicalDevice;
    m_memoryType	       = memoryType;
    m_heapIndex		       = heapIndex;
    m_bufferImageGranularity   = bufferImageGranularity;
    m_preferredBlockSize       = prefferedBlockSize;
    m_heapUsage		       = heapUsage;
    m_heapSizeLimit	       = heapSizeLimit;
    m_useMemoryBudgetExtension = useMemoryBudgetExtension;

    memset(m_blockSizes, 0, sizeof(m_blockSizes));
    memset(m_memory, 0, sizeof(m_memory));
    memset(m_mappedPtr, 0, sizeof(m_mappedPtr));
    memset(m_mapCount, 0, sizeof(m_mapCount));
    memset(m_allocators, 0, sizeof(m_allocators));
}

VkResult VulkanMemoryPool::Allocate(VkDeviceSize size, VkDeviceSize alignment, VulkanAllocationInfo& allocationInfo)
{
    for(size_t blockIndex = 0; blockIndex < MAX_BLOCKS; blockIndex++)
    {
	if(m_blockSizes[blockIndex] >= size && AllocateFromBlock(blockIndex, size, alignment, allocationInfo))
	{
	    return VK_SUCCESS;
	}
    }

    size_t blockIndex = ~static_cast<size_t>(0);
    for(size_t i = 0; i < MAX_BLOCKS; i++)
    {
	if(!m_blockSizes[i])
	{
	    blockIndex = i;
	    break;
	}
    }

    if(blockIndex >= MAX_BLOCKS)
    {
	return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    VkDeviceSize memoryBudget = 0;
    VkDeviceSize usedMemory   = 0;
    GetBudget(memoryBudget, usedMemory);

    VkDeviceSize maxInBudgetAllocSize = memoryBudget > usedMemory ? (memoryBudget - usedMemory) : 0;
    VkDeviceSize allocSize	      = MAX(size, MIN(maxInBudgetAllocSize, m_preferredBlockSize));

    VkMemoryAllocateInfo memoryAllocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    {
	memoryAllocateInfo.allocationSize  = allocSize;
	memoryAllocateInfo.memoryTypeIndex = m_memoryType;
    }

    if(vkAllocateMemory(m_device, &memoryAllocateInfo, nullptr, &m_memory[blockIndex]) != VK_SUCCESS)
    {
	return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    *m_heapUsage += memoryAllocateInfo.allocationSize;

    m_blockSizes[blockIndex]  = memoryAllocateInfo.allocationSize;
    char* tlsfAllocatorMemory = m_allocatorMemory + blockIndex * sizeof(TLSFAllocator);
    m_allocators[blockIndex]  = new(tlsfAllocatorMemory) TLSFAllocator(static_cast<uint32_t>(memoryAllocateInfo.allocationSize), static_cast<uint32_t>(m_bufferImageGranularity));

    if(AllocateFromBlock(blockIndex, size, alignment, allocationInfo))
    {
	return VK_SUCCESS;
    }

    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
}

VkResult VulkanMemoryPool::MapMemory(size_t blockIndex, VkDeviceSize offset, void** data)
{
    VkResult result = VK_SUCCESS;
    if(m_mapCount[blockIndex] == 0)
    {
	result = vkMapMemory(m_device, m_memory[blockIndex], 0, VK_WHOLE_SIZE, 0, &m_mappedPtr[blockIndex]);
    }

    if(result == VK_SUCCESS)
    {
	m_mapCount[blockIndex]++;
	*data = static_cast<char*>(m_mappedPtr[blockIndex]) + offset;
    }

    return result;
}

void VulkanMemoryPool::UnmapMemory(size_t blockIndex)
{
    ASSERT(m_mapCount[blockIndex]);
    m_mapCount[blockIndex]--;

    if(m_mapCount[blockIndex] == 0)
    {
	vkUnmapMemory(m_device, m_memory[blockIndex]);
	m_mappedPtr[blockIndex] = nullptr;
    }
}

void VulkanMemoryPool::GetBudget(VkDeviceSize& budget, VkDeviceSize& usage)
{
    if(m_useMemoryBudgetExtension)
    {
	VkPhysicalDeviceMemoryBudgetPropertiesEXT budgetProperties = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT };
	VkPhysicalDeviceMemoryProperties2 memoryProperties	   = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2, &budgetProperties };
	vkGetPhysicalDeviceMemoryProperties2(m_physicalDevice, &memoryProperties);

	budget = budgetProperties.heapBudget[m_heapIndex];
	usage  = budgetProperties.heapUsage[m_heapIndex];
    }
    else
    {
	budget = static_cast<VkDeviceSize>(m_heapSizeLimit * 0.8f);
	usage  = *m_heapUsage;
    }
}

bool VulkanMemoryPool::AllocateFromBlock(size_t blockIndex, VkDeviceSize size, VkDeviceSize alignment, VulkanAllocationInfo& allocationInfo)
{
    uint32_t freeSpace;
    uint32_t usedSpace;
    uint32_t wastedSpace;
    m_allocators[blockIndex]->GetFreeUsedWastedSizes(freeSpace, usedSpace, wastedSpace);

    if(size > freeSpace)
    {
	return false;
    }

    uint32_t offset;
    if(m_blockSizes[blockIndex] >= size && m_allocators[blockIndex]->Allocate(static_cast<uint32_t>(size), static_cast<uint32_t>(alignment), offset, allocationInfo.PoolData))
    {
	allocationInfo.Memory	  = m_memory[blockIndex];
	allocationInfo.Offset	  = offset;
	allocationInfo.Size	  = size;
	allocationInfo.MemoryType = m_memoryType;
	allocationInfo.PoolIndex  = m_memoryType;
	allocationInfo.BlockIndex = blockIndex;
	allocationInfo.MapCount	  = 0;

	return true;
    }

    return false;
}

VulkanMemoryAllocator::VulkanMemoryAllocator()
    : m_allocationInfoPool(256)
{
}

void VulkanMemoryAllocator::Initialize(VkDevice device, VkPhysicalDevice physicalDevice, bool useMemoryBudgetExtension)
{
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &m_memoryProperties);
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    m_device		       = device;
    m_physicalDevice	       = physicalDevice;
    m_bufferImageGranularity   = deviceProperties.limits.bufferImageGranularity;
    m_nonCoherentAtomSize      = deviceProperties.limits.nonCoherentAtomSize;
    m_useMemoryBudgetExtension = useMemoryBudgetExtension;

    for(size_t i = 0; i < m_memoryProperties.memoryHeapCount; ++i)
    {
	m_heapSizeLimits[i] = m_memoryProperties.memoryHeaps[i].size;
    }

    for(size_t i = 0; i < m_memoryProperties.memoryTypeCount; i++)
    {
	const uint32_t heapIndex = m_memoryProperties.memoryTypes[i].heapIndex;
	const auto& heap	 = m_memoryProperties.memoryHeaps[heapIndex];
	VkDeviceSize size	 = heap.size < MAX_BLOCK_SIZE ? heap.size : MAX_BLOCK_SIZE;
	m_pools[i].Initialize(m_device, m_physicalDevice, static_cast<uint32_t>(i), heapIndex, m_bufferImageGranularity, size, &m_heapUsage[heapIndex], m_heapSizeLimits[heapIndex], m_useMemoryBudgetExtension);
    }
}

VkResult VulkanMemoryAllocator::Allocate(const VulkanAllocationCreateInfo& allocationCreateInfo, const VkMemoryRequirements& memoryRequirements, const VkMemoryDedicatedAllocateInfo* dedicatedAllocateInfo, VulkanAllocationHandle& allocationHandle)
{
    uint32_t memoryTypeIndex;
    if(FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, allocationCreateInfo.RequiredFlags, allocationCreateInfo.PreferredFlags, memoryTypeIndex) != VK_SUCCESS)
    {
	return VK_ERROR_FEATURE_NOT_PRESENT;
    }

    VulkanAllocationInfo* allocationInfo = m_allocationInfoPool.Allocate();

    VkResult result = VK_SUCCESS;

    if(allocationCreateInfo.DedicatedAllocation)
    {
	VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, dedicatedAllocateInfo };
	{
	    allocateInfo.allocationSize	 = memoryRequirements.size;
	    allocateInfo.memoryTypeIndex = memoryTypeIndex;
	}

	result = vkAllocateMemory(m_device, &allocateInfo, nullptr, &allocationInfo->Memory);

	if(result == VK_SUCCESS)
	{
	    allocationInfo->Offset     = 0;
	    allocationInfo->Size       = memoryRequirements.size;
	    allocationInfo->MemoryType = memoryTypeIndex;
	    allocationInfo->PoolIndex  = ~static_cast<size_t>(0);
	    allocationInfo->BlockIndex = ~static_cast<size_t>(0);
	    allocationInfo->MapCount   = 0;
	    allocationInfo->PoolData   = nullptr;
	}
    }
    else
    {
	result = m_pools[memoryTypeIndex].Allocate(memoryRequirements.size, memoryRequirements.alignment, *allocationInfo);
    }

    if(result == VK_SUCCESS)
    {
	allocationHandle = reinterpret_cast<VulkanAllocationHandle>(allocationInfo);
    }
    else
    {
	m_allocationInfoPool.Free(allocationInfo);
    }

    return result;
}

VkResult VulkanMemoryAllocator::MapMemory(VulkanAllocationHandle allocationHandle, void** data)
{
    auto* allocationInfo = reinterpret_cast<VulkanAllocationInfo*>(allocationHandle);

    ASSERT(m_memoryProperties.memoryTypes[allocationInfo->MemoryType].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    VkResult result = VK_SUCCESS;

    if(allocationInfo->PoolIndex == ~static_cast<size_t>(0))
    {
	ASSERT(allocationInfo->BlockIndex == ~size_t(0));

	if(allocationInfo->MapCount == 0)
	{
	    result = vkMapMemory(m_device, allocationInfo->Memory, 0, VK_WHOLE_SIZE, 0, &allocationInfo->PoolData);
	}

	if(result == VK_SUCCESS)
	{
	    *data = static_cast<char*>(allocationInfo->PoolData);
	}
    }
    else
    {
	result = m_pools[allocationInfo->PoolIndex].MapMemory(allocationInfo->BlockIndex, allocationInfo->Offset, data);
    }

    if(result == VK_SUCCESS)
    {
	allocationInfo->MapCount++;
    }

    return result;
}

void VulkanMemoryAllocator::UnmapMemory(VulkanAllocationHandle allocationHandle)
{
    auto* allocationInfo = reinterpret_cast<VulkanAllocationInfo*>(allocationHandle);
    ASSERT(m_memoryProperties.memoryTypes[allocationInfo->MemoryType].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    ASSERT(allocationInfo->MapCount);
    allocationInfo->MapCount--;

    if(allocationInfo->PoolIndex == ~static_cast<size_t>(0))
    {
	ASSERT(allocationInfo->BlockIndex == ~size_t(0));

	if(allocationInfo->MapCount == 0)
	{
	    vkUnmapMemory(m_device, allocationInfo->Memory);
	    allocationInfo->PoolData = nullptr;
	}
    }
    else
    {
	m_pools[allocationInfo->PoolIndex].UnmapMemory(allocationInfo->BlockIndex);
    }
}

VulkanAllocationInfo VulkanMemoryAllocator::GetAllocationInfo(VulkanAllocationHandle allocationHandle)
{
    return *reinterpret_cast<VulkanAllocationInfo*>(allocationHandle);
}

VkResult VulkanMemoryAllocator::FindMemoryTypeIndex(uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties, VkMemoryPropertyFlags preferredProperties, uint32_t& memoryTypeIndex)
{
    memoryTypeIndex	   = ~static_cast<uint32_t>(0);
    int bestResultBitCount = -1;

    for(uint32_t memoryIndex = 0; memoryIndex < m_memoryProperties.memoryTypeCount; memoryIndex++)
    {
	const bool isRequiredMemoryType	       = memoryTypeBitsRequirement & (1 << memoryIndex);
	const VkMemoryPropertyFlags properties = m_memoryProperties.memoryTypes[memoryIndex].propertyFlags;
	const bool hasRequiredProperties       = (properties & requiredProperties) == requiredProperties;

	if(isRequiredMemoryType && hasRequiredProperties)
	{
	    uint32_t presentBits = properties & preferredProperties;

	    if(presentBits == preferredProperties)
	    {
		memoryTypeIndex = memoryIndex;
		return VK_SUCCESS;
	    }

	    int bitCount = 0;
	    for(uint32_t bit = 0; bit < 32; bit++)
	    {
		bitCount += (presentBits & (1 << bit)) >> bit;
	    }

	    if(bitCount > bestResultBitCount)
	    {
		bestResultBitCount = bitCount;
		memoryTypeIndex	   = memoryIndex;
	    }
	}
    }

    return memoryTypeIndex != ~uint32_t(0) ? VK_SUCCESS : VK_ERROR_FEATURE_NOT_PRESENT;
}
