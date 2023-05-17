//
// Created by Ploxie on 2023-05-17.
//

#include "VulkanFrameBufferCache.h"
#include "VulkanUtilities.h"

VulkanFrameBufferCache::VulkanFrameBufferCache(VkDevice device)
    : m_device(device)
{
}
VulkanFrameBufferCache::~VulkanFrameBufferCache()
{
    for(auto& buffer: m_frameBuffers)
    {
	vkDestroyFramebuffer(m_device, buffer.second, nullptr);
    }
}
VkFramebuffer VulkanFrameBufferCache::GetFrameBuffer(const VulkanFrameBufferDescription& frameBufferDescription)
{
    VkFramebuffer& frameBuffer = m_frameBuffers[frameBufferDescription];
    if(frameBuffer != VK_NULL_HANDLE)
    {
	return frameBuffer;
    }

    VkFramebufferAttachmentImageInfo imageInfos[9];
    for(size_t i = 0; i < frameBufferDescription.m_attachmentCount; i++)
    {
	const auto& attachmentInfo = frameBufferDescription.m_attachmentInfos[i];
	auto& info		   = imageInfos[i];

	info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO };
	{
	    info.flags		 = attachmentInfo.Flags;
	    info.usage		 = attachmentInfo.Usage;
	    info.width		 = attachmentInfo.Width;
	    info.height		 = attachmentInfo.Height;
	    info.layerCount	 = attachmentInfo.LayerCount;
	    info.viewFormatCount = 1;
	    info.pViewFormats	 = &attachmentInfo.Format;
	}
    }

    VkFramebufferAttachmentsCreateInfo attachmentsCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO };
    {
	attachmentsCreateInfo.attachmentImageInfoCount = frameBufferDescription.m_attachmentCount;
	attachmentsCreateInfo.pAttachmentImageInfos    = imageInfos;
    }

    VkFramebufferCreateInfo frameBufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    {
	frameBufferCreateInfo.pNext	      = &attachmentsCreateInfo;
	frameBufferCreateInfo.flags	      = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
	frameBufferCreateInfo.renderPass      = frameBufferDescription.m_renderPass;
	frameBufferCreateInfo.attachmentCount = frameBufferDescription.m_attachmentCount;
	frameBufferCreateInfo.pAttachments    = nullptr;
	frameBufferCreateInfo.width	      = frameBufferDescription.m_width;
	frameBufferCreateInfo.height	      = frameBufferDescription.m_height;
	frameBufferCreateInfo.layers	      = frameBufferDescription.m_layers;
    }

    VulkanUtilities::checkResult(vkCreateFramebuffer(m_device, &frameBufferCreateInfo, nullptr, &frameBuffer));

    return frameBuffer;
}
