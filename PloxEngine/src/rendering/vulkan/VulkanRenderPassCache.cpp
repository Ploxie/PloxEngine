//
// Created by Ploxie on 2023-05-17.
//
#include "VulkanRenderPassCache.h"
#include "VulkanUtilities.h"
VulkanRenderPassCache::VulkanRenderPassCache(VkDevice device)
    : m_device(device)
{
}
VulkanRenderPassCache::~VulkanRenderPassCache()
{
    for(auto& pass: m_renderPasses)
    {
	vkDestroyRenderPass(m_device, pass.second, nullptr);
    }
}
VkRenderPass VulkanRenderPassCache::GetRenderPass(const VulkanRenderPassDescription& renderPassDescription)
{
    VkRenderPass& pass = m_renderPasses[renderPassDescription];

    if(pass != VK_NULL_HANDLE)
    {
	return pass;
    }

    uint32_t attachmentCount				  = 0;
    VkAttachmentDescription attachmentDescriptions[9]	  = {};
    VkAttachmentReference colorAttachmentReferences[8]	  = {};
    VkAttachmentReference depthStencilAttachmentReference = {};

    for(uint32_t i = 0; i < renderPassDescription.m_colorAttachmentCount; i++)
    {
	const auto& attachment = renderPassDescription.m_colorAttachments[i];

	colorAttachmentReferences[i] = { i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

	auto& attachmentDesc = attachmentDescriptions[i];
	attachmentDesc	     = {};
	{
	    attachmentDesc.format	  = attachment.Format;
	    attachmentDesc.samples	  = attachment.Samples;
	    attachmentDesc.loadOp	  = attachment.LoadOp;
	    attachmentDesc.storeOp	  = attachment.StoreOp;
	    attachmentDesc.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	    attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	    attachmentDesc.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	    attachmentDesc.finalLayout	  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
	attachmentCount++;
    }

    if(renderPassDescription.m_depthStencilAttachmentPresent)
    {
	const auto& attachment = renderPassDescription.m_depthStencilAttachment;

	depthStencilAttachmentReference = { attachmentCount, attachment.Layout };

	auto& attachmentDesc = attachmentDescriptions[attachmentCount];
	attachmentDesc	     = {};
	{
	    attachmentDesc.format	  = attachment.Format;
	    attachmentDesc.samples	  = attachment.Samples;
	    attachmentDesc.loadOp	  = attachment.LoadOp;
	    attachmentDesc.storeOp	  = attachment.StoreOp;
	    attachmentDesc.stencilLoadOp  = attachment.StencilLoadOp;
	    attachmentDesc.stencilStoreOp = attachment.StencilStoreOp;
	    attachmentDesc.initialLayout  = attachment.Layout;
	    attachmentDesc.finalLayout	  = attachment.Layout;
	}
	attachmentCount++;
    }

    VkSubpassDescription subPassDescription = {};
    {
	subPassDescription.pipelineBindPoint	   = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subPassDescription.inputAttachmentCount	   = 0;
	subPassDescription.pInputAttachments	   = nullptr;
	subPassDescription.colorAttachmentCount	   = renderPassDescription.m_colorAttachmentCount;
	subPassDescription.pColorAttachments	   = renderPassDescription.m_colorAttachmentCount > 0 ? colorAttachmentReferences : nullptr;
	subPassDescription.pResolveAttachments	   = nullptr;
	subPassDescription.pDepthStencilAttachment = renderPassDescription.m_depthStencilAttachmentPresent ? &depthStencilAttachmentReference : nullptr;
	subPassDescription.preserveAttachmentCount = 0;
	subPassDescription.pPreserveAttachments	   = nullptr;
    }

    VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    {
	renderPassInfo.attachmentCount = attachmentCount;
	renderPassInfo.pAttachments    = attachmentDescriptions;
	renderPassInfo.subpassCount    = 1;
	renderPassInfo.pSubpasses      = &subPassDescription;
	renderPassInfo.dependencyCount = 0;
	renderPassInfo.pDependencies   = nullptr;
    }

    VulkanUtilities::checkResult(vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &pass));

    return pass;
}
size_t VulkanRenderPassDescriptionHash::operator()(const VulkanRenderPassDescription& value) const
{
    return value.m_hashValue;
}
