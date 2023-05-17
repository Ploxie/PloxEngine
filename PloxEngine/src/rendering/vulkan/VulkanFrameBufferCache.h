//
// Created by Ploxie on 2023-05-17.
//

#pragma once
#include "EASTL/fixed_hash_map.h"
#include "vulkan/vulkan.h"
#include "VulkanFrameBufferDescription.h"

struct VulkanFrameBufferDescriptionHash
{
    size_t operator()(const VulkanFrameBufferDescription &value) const;
};

class VulkanFrameBufferCache
{
public:
    explicit VulkanFrameBufferCache(VkDevice device);
    VulkanFrameBufferCache(VulkanFrameBufferCache &)		       = delete;
    VulkanFrameBufferCache(VulkanFrameBufferCache &&)		       = delete;
    VulkanFrameBufferCache &operator=(const VulkanFrameBufferCache &)  = delete;
    VulkanFrameBufferCache &operator=(const VulkanFrameBufferCache &&) = delete;
    ~VulkanFrameBufferCache();

    VkFramebuffer GetFrameBuffer(const VulkanFrameBufferDescription &frameBufferDescription);

private:
    eastl::fixed_hash_map<VulkanFrameBufferDescription, VkFramebuffer, 64, 65, true, VulkanFrameBufferDescriptionHash> m_frameBuffers;
    VkDevice m_device;
};