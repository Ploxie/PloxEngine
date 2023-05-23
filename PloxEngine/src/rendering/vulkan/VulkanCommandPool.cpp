//
// Created by Ploxie on 2023-05-19.
//

#include "VulkanCommandPool.h"
#include "volk.h"
#include "VulkanGraphicsAdapter.h"
#include "VulkanUtilities.h"

VulkanCommandPool::VulkanCommandPool(VulkanGraphicsAdapter* adapter, const VulkanQueue& queue)
    : m_adapter(adapter), m_commandPool(VK_NULL_HANDLE), m_commandMemoryPool(sizeof(VulkanCommand), 32, "VulkanCommand Pool Allocator")
{
    VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    {
	createInfo.flags	    = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	createInfo.queueFamilyIndex = queue.GetQueueFamily();
    }

    VulkanUtilities::checkResult(vkCreateCommandPool(adapter->GetDevice(), &createInfo, nullptr, &m_commandPool));
}

VulkanCommandPool::~VulkanCommandPool()
{
    vkDestroyCommandPool(m_adapter->GetDevice(), m_commandPool, nullptr);

    for(auto* commands: m_liveCommands)
    {
	ALLOC_DELETE(&m_commandMemoryPool, commands);
    }

    m_liveCommands.clear();
}

void* VulkanCommandPool::GetNativeHandle() const
{
    return m_commandPool;
}

void VulkanCommandPool::Allocate(uint32_t count, Command** commands)
{
    constexpr uint32_t BATCH_SIZE = 32;
    const uint32_t iterations	  = (count + (BATCH_SIZE - 1)) / BATCH_SIZE;

    VkDevice device = m_adapter->GetDevice();
    for(uint32_t i = 0; i < iterations; i++)
    {
	uint32_t countVk = MIN(BATCH_SIZE, count - i * BATCH_SIZE);
	VkCommandBuffer commandBuffers[BATCH_SIZE];
	VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	{
	    allocateInfo.commandPool	    = m_commandPool;
	    allocateInfo.level		    = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	    allocateInfo.commandBufferCount = countVk;
	}
	vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers);

	for(uint32_t j = 0; j < countVk; j++)
	{
	    auto* command		 = ALLOC_NEW(&m_commandMemoryPool, VulkanCommand)(commandBuffers[j], m_adapter);
	    commands[j + i * BATCH_SIZE] = command;

	    m_liveCommands.push_back(command);
	}
    }
}

void VulkanCommandPool::Free(uint32_t count, Command** commands)
{
    constexpr uint32_t BATCH_SIZE = 32;
    const uint32_t iterations	  = (count + (BATCH_SIZE - 1)) / BATCH_SIZE;

    VkDevice device = m_adapter->GetDevice();
    for(uint32_t i = 0; i < iterations; i++)
    {
	uint32_t countVk = MIN(BATCH_SIZE, count - i * BATCH_SIZE);
	VkCommandBuffer commandBuffers[BATCH_SIZE];

	for(uint32_t j = 0; j < countVk; j++)
	{
	    auto* command = dynamic_cast<VulkanCommand*>(commands[j + i * BATCH_SIZE]);
	    ASSERT(commands);
	    commandBuffers[j] = static_cast<VkCommandBuffer>(command->GetNativeHandle());

	    ALLOC_DELETE(&m_commandMemoryPool, command);

	    auto* it = eastl::find(m_liveCommands.begin(), m_liveCommands.end(), command);
	    if(it != m_liveCommands.end())
	    {
		eastl::swap(m_liveCommands.back(), *it);
		m_liveCommands.erase(m_liveCommands.end() - 1);
	    }
	}

	vkFreeCommandBuffers(device, m_commandPool, countVk, commandBuffers);
    }
}

void VulkanCommandPool::Reset()
{
    vkResetCommandPool(m_adapter->GetDevice(), m_commandPool, 0);
}
