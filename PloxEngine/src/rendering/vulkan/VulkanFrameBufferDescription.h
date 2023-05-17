//
// Created by Ploxie on 2023-05-17.
//
#pragma once
#include "vulkan/vulkan.h"

class VulkanFrameBufferDescription
{
public:
    friend class VulkanFrameBufferCache;
    friend class VulkanFrameBufferDescriptionHash;

    struct AttachmentInfo
    {
	VkImageCreateFlags Flags;
	VkImageUsageFlags Usage;
	uint32_t Width;
	uint32_t Height;
	uint32_t LayerCount;
	VkFormat Format;
    };

    explicit VulkanFrameBufferDescription();
    void SetRenderPass(VkRenderPass renderPass);
    void SetAttachment(uint32_t count, const AttachmentInfo* attachmentInfo);
    void SetExtent(uint32_t width, uint32_t height, uint32_t layers);
    void Finalize();

private:
    AttachmentInfo m_attachmentInfos[9];
    VkRenderPass m_renderPass;
    uint32_t m_attachmentCount;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_layers;
    size_t m_hashValue;
};

bool operator==(const VulkanFrameBufferDescription& left, const VulkanFrameBufferDescription& right);
