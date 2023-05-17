//
// Created by Ploxie on 2023-05-17.
//

#pragma once
#include "eastl/fixed_hash_map.h"
#include "vulkan/vulkan.h"
#include "VulkanRenderPassDescription.h"

struct VulkanRenderPassDescriptionHash
{
    size_t operator()(const VulkanRenderPassDescription &value) const;
};

class VulkanRenderPassCache
{
public:
    explicit VulkanRenderPassCache(VkDevice device);
    VulkanRenderPassCache(VulkanRenderPassCache &)		     = delete;
    VulkanRenderPassCache(VulkanRenderPassCache &&)		     = delete;
    VulkanRenderPassCache &operator=(const VulkanRenderPassCache &)  = delete;
    VulkanRenderPassCache &operator=(const VulkanRenderPassCache &&) = delete;
    ~VulkanRenderPassCache();
    VkRenderPass GetRenderPass(const VulkanRenderPassDescription &renderPassDescription);

private:
    eastl::fixed_hash_map<VulkanRenderPassDescription, VkRenderPass, 64, 65, true, VulkanRenderPassDescriptionHash> m_renderPasses;
    VkDevice m_device;
};