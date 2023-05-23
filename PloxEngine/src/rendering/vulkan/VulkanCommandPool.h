//
// Created by Ploxie on 2023-05-19.
//

#pragma once
#include "EASTL/fixed_vector.h"
#include "rendering/types/CommandPool.h"
#include "utility/memory/PoolAllocator.h"
#include "vulkan/vulkan.h"
#include "VulkanCommand.h"

class VulkanGraphicsAdapter;
class VulkanQueue;

class VulkanCommandPool : public CommandPool
{
public:
    explicit VulkanCommandPool(VulkanGraphicsAdapter* adapter, const VulkanQueue& queue);
    VulkanCommandPool(CommandPool&)			    = delete;
    VulkanCommandPool(CommandPool&&)			    = delete;
    VulkanCommandPool& operator=(const VulkanCommandPool&)  = delete;
    VulkanCommandPool& operator=(const VulkanCommandPool&&) = delete;
    ~VulkanCommandPool() override;

    void* GetNativeHandle() const override;
    void Allocate(uint32_t count, Command** commands) override;
    void Free(uint32_t count, Command** commands) override;
    void Reset() override;

private:
    VulkanGraphicsAdapter* m_adapter;
    VkCommandPool m_commandPool;
    DynamicPoolAllocator m_commandMemoryPool;
    eastl::fixed_vector<VulkanCommand*, 32> m_liveCommands;
};