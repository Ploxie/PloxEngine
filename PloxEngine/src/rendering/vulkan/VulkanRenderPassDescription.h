//
// Created by Ploxie on 2023-05-17.
//

#pragma once
#include "vulkan/vulkan.h"

class VulkanRenderPassDescription
{
public:
    friend class VulkanRenderPassCache;
    friend class VulkanRenderPassDescriptionHash;

    struct ColorAttachmentDescription
    {
	VkFormat Format;
	VkSampleCountFlagBits Samples;
	VkAttachmentLoadOp LoadOp;
	VkAttachmentStoreOp StoreOp;
    };

    struct DepthStencilAttachmentDescription
    {
	VkFormat Format;
	VkSampleCountFlagBits Samples;
	VkAttachmentLoadOp LoadOp;
	VkAttachmentStoreOp StoreOp;
	VkAttachmentLoadOp StencilLoadOp;
	VkAttachmentStoreOp StencilStoreOp;
	VkImageLayout Layout;
    };

    explicit VulkanRenderPassDescription();
    void SetColorAttachment(uint32_t count, const ColorAttachmentDescription* colorAttachments);
    void SetDepthStencilAttachment(const DepthStencilAttachmentDescription& depthStencilAttachment);
    void Finalize();

private:
    ColorAttachmentDescription m_colorAttachments[8];
    DepthStencilAttachmentDescription m_depthStencilAttachment;
    uint32_t m_colorAttachmentCount;
    VkBool32 m_depthStencilAttachmentPresent;
    size_t m_hashValue;
};

bool operator==(const VulkanRenderPassDescription& left, const VulkanRenderPassDescription& right);