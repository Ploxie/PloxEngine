//
// Created by Ploxie on 2023-05-17.
//
#include "VulkanCommand.h"
#include "core/Assert.h"
#include "rendering/RenderUtilities.h"
#include "rendering/types/Barrier.h"
#include "utility/memory/DefaultAllocator.h"
#include "volk.h"
#include "VulkanGraphicsAdapter.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanUtilities.h"

static constexpr uint32_t MEMORY_SIZE = 1024 * 256;

struct ResourceStateInfo
{
    VkPipelineStageFlags m_stageMask;
    VkAccessFlags m_accessMask;
    VkImageLayout m_layout;
    bool m_readAccess;
    bool m_writeAccess;
};

static ResourceStateInfo GetResourceStateInfo(ResourceState state, VkPipelineStageFlags stageFlags, bool isImage, Format imageFormat)
{
    ResourceStateInfo result {};

    if(state == ResourceState::UNDEFINED)
    {
	result = { VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, VK_IMAGE_LAYOUT_UNDEFINED, false, false };
	return result;
    }

    if((state & ResourceState::READ_RESOURCE) != 0)
    {
	result.m_stageMask |= stageFlags;
	result.m_accessMask |= VK_ACCESS_SHADER_READ_BIT;
	result.m_layout	    = RenderUtilities::IsDepthFormat(imageFormat) || RenderUtilities::IsStencilFormat(imageFormat) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	result.m_readAccess = true;
    }

    if((state & ResourceState::READ_DEPTH_STENCIL) != 0)
    {
	ASSERT(isImage);
	result.m_stageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	result.m_accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	result.m_layout	    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	result.m_readAccess = true;
    }

    if((state & ResourceState::READ_CONSTANT_BUFFER) != 0)
    {
	ASSERT(!isImage);
	result.m_stageMask |= stageFlags;
	result.m_accessMask |= VK_ACCESS_UNIFORM_READ_BIT;
	result.m_readAccess = true;
    }

    if((state & ResourceState::READ_VERTEX_BUFFER) != 0)
    {
	ASSERT(!isImage);
	result.m_stageMask |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
	result.m_accessMask |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	result.m_readAccess = true;
    }

    if((state & ResourceState::READ_INDEX_BUFFER) != 0)
    {
	ASSERT(!isImage);
	result.m_stageMask |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
	result.m_accessMask |= VK_ACCESS_INDEX_READ_BIT;
	result.m_readAccess = true;
    }

    if((state & ResourceState::READ_INDIRECT_BUFFER) != 0)
    {
	ASSERT(!isImage);
	result.m_stageMask |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
	result.m_accessMask |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
	result.m_readAccess = true;
    }

    if((state & ResourceState::READ_TRANSFER) != 0)
    {
	ASSERT(!isImage || state == ResourceState::READ_TRANSFER); // READ_TRANSFER is an exclusive state on image resources
	result.m_stageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	result.m_accessMask |= VK_ACCESS_TRANSFER_READ_BIT;
	result.m_layout	    = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	result.m_readAccess = true;
    }

    if((state & ResourceState::WRITE_DEPTH_STENCIL) != 0)
    {
	ASSERT(isImage);
	ASSERT(state == ResourceState::WRITE_DEPTH_STENCIL);
	result.m_stageMask   = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	result.m_accessMask  = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	result.m_layout	     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	result.m_writeAccess = true;
    }

    if((state & ResourceState::WRITE_COLOR_ATTACHMENT) != 0)
    {
	ASSERT(isImage);
	ASSERT(state == ResourceState::WRITE_COLOR_ATTACHMENT);
	result.m_stageMask   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	result.m_accessMask  = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	result.m_layout	     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	result.m_writeAccess = true;
    }

    if((state & ResourceState::WRITE_TRANSFER) != 0)
    {
	ASSERT(state == ResourceState::WRITE_TRANSFER);
	result.m_stageMask   = VK_PIPELINE_STAGE_TRANSFER_BIT;
	result.m_accessMask  = VK_ACCESS_TRANSFER_WRITE_BIT;
	result.m_layout	     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	result.m_writeAccess = true;
    }

    if((state & ResourceState::CLEAR_RESOURCE) != 0)
    {
	ASSERT(state == ResourceState::CLEAR_RESOURCE);
	result.m_stageMask   = VK_PIPELINE_STAGE_TRANSFER_BIT;
	result.m_accessMask  = VK_ACCESS_TRANSFER_WRITE_BIT;
	result.m_layout	     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	result.m_writeAccess = true;
    }

    if((state & ResourceState::RW_RESOURCE) != 0)
    {
	ASSERT(state == ResourceState::RW_RESOURCE);
	result.m_stageMask   = stageFlags;
	result.m_accessMask  = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	result.m_layout	     = VK_IMAGE_LAYOUT_GENERAL;
	result.m_readAccess  = true;
	result.m_writeAccess = true;
    }

    if((state & ResourceState::RW_RESOURCE_READ_ONLY) != 0)
    {
	ASSERT(state == ResourceState::RW_RESOURCE_READ_ONLY);
	result.m_stageMask   = stageFlags;
	result.m_accessMask  = VK_ACCESS_SHADER_READ_BIT;
	result.m_layout	     = VK_IMAGE_LAYOUT_GENERAL;
	result.m_readAccess  = true;
	result.m_writeAccess = false;
    }

    if((state & ResourceState::RW_RESOURCE_WRITE_ONLY) != 0)
    {
	ASSERT(state == ResourceState::RW_RESOURCE_WRITE_ONLY);
	result.m_stageMask   = stageFlags;
	result.m_accessMask  = VK_ACCESS_SHADER_WRITE_BIT;
	result.m_layout	     = VK_IMAGE_LAYOUT_GENERAL;
	result.m_readAccess  = false;
	result.m_writeAccess = true;
    }

    if((state & ResourceState::PRESENT) != 0)
    {
	ASSERT(isImage);
	ASSERT(state == ResourceState::PRESENT);
	result.m_stageMask   = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	result.m_accessMask  = 0;
	result.m_layout	     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	result.m_readAccess  = true;
	result.m_writeAccess = false;
    }

    result.m_layout = isImage ? result.m_layout : VK_IMAGE_LAYOUT_UNDEFINED;

    return result;
};

VulkanCommand::VulkanCommand(VkCommandBuffer commandBuffer, VulkanGraphicsAdapter* adapter) noexcept
    : m_commandBuffer(commandBuffer), m_adapter(adapter), m_memoryAllocation(DefaultAllocator::Get(), DefaultAllocator::Get()->allocate(MEMORY_SIZE), MEMORY_SIZE), m_allocator(static_cast<char*>(m_memoryAllocation.GetAllocation()), MEMORY_SIZE, "VulkanCommand Linear Allocator")
{
}

void* VulkanCommand::GetNativeHandle() const
{
    return m_commandBuffer;
}

void VulkanCommand::Begin()
{
    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    {
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }
    vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
}

void VulkanCommand::End()
{
    vkEndCommandBuffer(m_commandBuffer);
}

void VulkanCommand::BindPipeline(const GraphicsPipeline* pipeline)
{
    const auto* pipelineVk = dynamic_cast<const VulkanGraphicsPipeline*>(pipeline);
    ASSERT(pipelineVk);

    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VkPipeline>(pipeline->GetNativeHandle()));
    pipelineVk->BindStaticSamplerSet(m_commandBuffer);
}

void VulkanCommand::SetViewports(uint32_t firstViewport, uint32_t viewportCount, const Viewport* viewports)
{
    vkCmdSetViewport(m_commandBuffer, firstViewport, viewportCount, reinterpret_cast<const VkViewport*>(viewports));
}

void VulkanCommand::SetScissors(uint32_t firstScissor, uint32_t scissorCount, const Rect* scissors)
{
    vkCmdSetScissor(m_commandBuffer, firstScissor, scissorCount, reinterpret_cast<const VkRect2D*>(scissors));
}

void VulkanCommand::SetLineWidth(float lineWidth)
{
    vkCmdSetLineWidth(m_commandBuffer, lineWidth);
}

void VulkanCommand::SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    vkCmdSetDepthBias(m_commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void VulkanCommand::SetBlendConstants(const float blendConstants[4])
{
    vkCmdSetBlendConstants(m_commandBuffer, blendConstants);
}

void VulkanCommand::SetDepthBounds(float minDepthBounds, float maxDepthBounds)
{
    vkCmdSetDepthBounds(m_commandBuffer, minDepthBounds, maxDepthBounds);
}

void VulkanCommand::SetStencilCompareMask(StencilFaceFlags faceMask, uint32_t compareMask)
{
    vkCmdSetStencilCompareMask(m_commandBuffer, VulkanUtilities::Translate(faceMask), compareMask);
}

void VulkanCommand::SetStencilWriteMask(StencilFaceFlags faceMask, uint32_t writeMask)
{
    vkCmdSetStencilWriteMask(m_commandBuffer, VulkanUtilities::Translate(faceMask), writeMask);
}

void VulkanCommand::SetStencilReference(StencilFaceFlags faceMask, uint32_t reference)
{
    vkCmdSetStencilReference(m_commandBuffer, VulkanUtilities::Translate(faceMask), reference);
}

void VulkanCommand::BindDescriptorSets(const GraphicsPipeline* pipeline, uint32_t firstSet, uint32_t count, const DescriptorSet* const* sets, uint32_t offsetCount, uint32_t* offsets)
{
    LinearAllocatorFrame allocatorFrame(&m_allocator);

    const auto* pipelineVk = dynamic_cast<const VulkanGraphicsPipeline*>(pipeline);
    ASSERT(pipelineVk);

    VkPipelineLayout pipelineLayout = pipelineVk->GetLayout();

    auto* descriptorSets = allocatorFrame.AllocateArray<VkDescriptorSet>(count);

    for(size_t i = 0; i < count; i++)
    {
	descriptorSets[i] = static_cast<VkDescriptorSet>(sets[i]->GetNativeHandle());
    }

    vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, firstSet, count, descriptorSets, offsetCount, offsets);
}

void VulkanCommand::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    vkCmdDraw(m_commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommand::ClearColorImage(const Image* image, const ClearColorValue* color, uint32_t rangeCount, const ImageSubresourceRange* ranges)
{
    LinearAllocatorFrame allocatorFrame(&m_allocator);

    auto* const imageVk = static_cast<VkImage>(image->GetNativeHandle());
    auto* rangesVk	= allocatorFrame.AllocateArray<VkImageSubresourceRange>(rangeCount);

    for(size_t i = 0; i < rangeCount; ++i)
    {
	const auto& range = ranges[i];
	rangesVk[i]	  = { VK_IMAGE_ASPECT_COLOR_BIT, range.BaseMipLevel, range.LevelCount, range.BaseArrayLayer, range.LayerCount };
    }

    vkCmdClearColorImage(m_commandBuffer, imageVk, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, reinterpret_cast<const VkClearColorValue*>(color), rangeCount, rangesVk);
}

void VulkanCommand::ClearDepthStencilImage(const Image* image, const ClearDepthStencilValue* depthStencil, uint32_t rangeCount, const ImageSubresourceRange* ranges)
{
    LinearAllocatorFrame allocatorFrame(&m_allocator);

    auto* const imageVk			     = static_cast<VkImage>(image->GetNativeHandle());
    const VkImageAspectFlags imageAspectMask = VulkanUtilities::GetImageAspectMask(VulkanUtilities::Translate(image->GetDescription().Format));
    auto* rangesVk			     = allocatorFrame.AllocateArray<VkImageSubresourceRange>(rangeCount);

    for(size_t i = 0; i < rangeCount; ++i)
    {
	const auto& range = ranges[i];
	rangesVk[i]	  = { imageAspectMask, range.BaseMipLevel, range.LevelCount, range.BaseArrayLayer, range.LayerCount };
    }

    vkCmdClearDepthStencilImage(m_commandBuffer, imageVk, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, reinterpret_cast<const VkClearDepthStencilValue*>(depthStencil), rangeCount, rangesVk);
}

void VulkanCommand::Barrier(uint32_t count, const class Barrier* barriers)
{
    LinearAllocatorFrame allocatorFrame(&m_allocator);

    uint32_t imageBarrierCount	= 0;
    uint32_t bufferBarrierCount = 0;

    auto* imageBarriers		   = allocatorFrame.AllocateArray<VkImageMemoryBarrier>(count);
    auto* bufferBarriers	   = allocatorFrame.AllocateArray<VkBufferMemoryBarrier>(count);
    VkMemoryBarrier memoryBarrier  = { VK_STRUCTURE_TYPE_MEMORY_BARRIER };
    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dstStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    for(size_t i = 0; i < count; i++)
    {
	const auto& barrier = barriers[i];
	ASSERT((bool) barrier.m_image != (bool) barrier.m_buffer);

	if((barrier.m_flags & BarrierFlags::BARRIER_BEGIN) != 0)
	{
	    continue;
	}

	const auto imageFormat	   = barrier.m_image ? barrier.m_image->GetDescription().Format : Format::UNDEFINED;
	const auto beforeStateInfo = GetResourceStateInfo(barrier.m_stateBefore, VulkanUtilities::Translate(barrier.m_stagesBefore), bool(barrier.m_image), imageFormat);
	const auto afterStateInfo  = GetResourceStateInfo(barrier.m_stateAfter, VulkanUtilities::Translate(barrier.m_stagesAfter), bool(barrier.m_image), imageFormat);

	const bool queueAcquire = (barrier.m_flags & BarrierFlags::QUEUE_OWNERSHIP_AQUIRE) != 0;
	const bool queueRelease = (barrier.m_flags & BarrierFlags::QUEUE_OWNERSHIP_RELEASE) != 0;

	const bool imageBarrierRequired	    = barrier.m_image && (beforeStateInfo.m_layout != afterStateInfo.m_layout || queueAcquire || queueRelease);
	const bool bufferBarrierRequired    = barrier.m_buffer && (queueAcquire || queueRelease);
	const bool memoryBarrierRequired    = beforeStateInfo.m_writeAccess && !imageBarrierRequired && !bufferBarrierRequired;
	const bool executionBarrierRequired = beforeStateInfo.m_writeAccess || afterStateInfo.m_writeAccess || memoryBarrierRequired || bufferBarrierRequired || imageBarrierRequired;

	const auto* srcQueue = dynamic_cast<const VulkanQueue*>(barrier.m_srcQueue);
	const auto* dstQueue = dynamic_cast<const VulkanQueue*>(barrier.m_dstQueue);

	if(imageBarrierRequired)
	{
	    const auto& subResRange		     = barrier.m_imageSubresourceRange;
	    const VkImageAspectFlags imageAspectMask = VulkanUtilities::GetImageAspectMask(VulkanUtilities::Translate(barrier.m_image->GetDescription().Format));

	    auto& imageBarrier = imageBarriers[imageBarrierCount++];
	    imageBarrier       = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	    {
		imageBarrier.srcAccessMask	 = queueAcquire ? 0 : beforeStateInfo.m_accessMask;
		imageBarrier.dstAccessMask	 = queueRelease ? 0 : afterStateInfo.m_accessMask;
		imageBarrier.oldLayout		 = beforeStateInfo.m_layout;
		imageBarrier.newLayout		 = afterStateInfo.m_layout;
		imageBarrier.srcQueueFamilyIndex = barrier.m_srcQueue ? srcQueue->GetQueueFamily() : VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.dstQueueFamilyIndex = barrier.m_dstQueue ? dstQueue->GetQueueFamily() : VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.image		 = static_cast<VkImage>(barrier.m_image->GetNativeHandle());
		imageBarrier.subresourceRange	 = { imageAspectMask, subResRange.BaseMipLevel, subResRange.LevelCount, subResRange.BaseArrayLayer, subResRange.LayerCount };
	    }
	}
	else if(bufferBarrierRequired)
	{
	    auto& bufferBarrier = bufferBarriers[bufferBarrierCount++];
	    bufferBarrier	= { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	    {
		bufferBarrier.srcAccessMask	  = queueAcquire ? 0 : beforeStateInfo.m_accessMask;
		bufferBarrier.dstAccessMask	  = queueRelease ? 0 : afterStateInfo.m_accessMask;
		bufferBarrier.srcQueueFamilyIndex = barrier.m_srcQueue ? srcQueue->GetQueueFamily() : VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.dstQueueFamilyIndex = barrier.m_dstQueue ? dstQueue->GetQueueFamily() : VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.buffer		  = static_cast<VkBuffer>(barrier.m_buffer->GetNativeHandle());
		bufferBarrier.offset		  = 0;
		bufferBarrier.size		  = VK_WHOLE_SIZE;
	    }
	}

	if(memoryBarrierRequired)
	{
	    memoryBarrier.srcAccessMask = beforeStateInfo.m_accessMask;
	    memoryBarrier.dstAccessMask = afterStateInfo.m_accessMask;
	}

	if(executionBarrierRequired)
	{
	    srcStages |= queueAcquire ? VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT : beforeStateInfo.m_stageMask;
	    dstStages |= queueRelease ? VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT : afterStateInfo.m_stageMask;
	}
    }

    if(bufferBarrierCount || imageBarrierCount || memoryBarrier.srcAccessMask || srcStages != VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT || dstStages != VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
    {
	vkCmdPipelineBarrier(m_commandBuffer, srcStages, dstStages, 0, 1, &memoryBarrier, bufferBarrierCount, bufferBarriers, imageBarrierCount, imageBarriers);
    }
}

void VulkanCommand::PushConstants(const GraphicsPipeline* pipeline, ShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* values)
{
    const auto* pipelineVk = dynamic_cast<const VulkanGraphicsPipeline*>(pipeline);
    ASSERT(pipelineVk);
    vkCmdPushConstants(m_commandBuffer, pipelineVk->GetLayout(), VulkanUtilities::Translate(stageFlags), offset, size, values);
}

void VulkanCommand::BeginRenderPass(uint32_t colorAttachmentCount, ColorAttachmentDescription* colorAttachments, DepthStencilAttachmentDescription* depthStencilAttachment, const Rect& renderArea, bool rwTextureBufferAccess)
{
    ASSERT(colorAttachmentCount <= 8);

    if(m_adapter->IsDynamicRenderingExtensionSupported())
    {
	eastl::fixed_vector<VkRenderingAttachmentInfoKHR, 8> colorAttachmentsVk;
	for(size_t i = 0; i < colorAttachmentCount; i++)
	{
	    const auto& colorAttachment = colorAttachments[i];

	    VkRenderingAttachmentInfoKHR attachment = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR };
	    {
		attachment.imageView	    = static_cast<VkImageView>(colorAttachment.ImageView->GetNativeHandle());
		attachment.imageLayout	    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment.resolveMode	    = VK_RESOLVE_MODE_NONE;
		attachment.loadOp	    = VulkanUtilities::Translate(colorAttachment.LoadOp);
		attachment.storeOp	    = VulkanUtilities::Translate(colorAttachment.StoreOp);
		attachment.clearValue.color = *reinterpret_cast<const VkClearColorValue*>(&colorAttachment.ClearValue);
	    }
	    colorAttachmentsVk.push_back(attachment);
	}

	VkRenderingAttachmentInfoKHR depthAttachment   = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR };
	VkRenderingAttachmentInfoKHR stencilAttachment = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR };
	if(depthStencilAttachment)
	{
	    depthAttachment.imageView		    = static_cast<VkImageView>(depthStencilAttachment->ImageView->GetNativeHandle());
	    depthAttachment.imageLayout		    = depthStencilAttachment->ReadOnly ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	    depthAttachment.resolveMode		    = VK_RESOLVE_MODE_NONE;
	    depthAttachment.loadOp		    = VulkanUtilities::Translate(depthStencilAttachment->LoadOp);
	    depthAttachment.storeOp		    = VulkanUtilities::Translate(depthStencilAttachment->StoreOp);
	    depthAttachment.clearValue.depthStencil = *reinterpret_cast<const VkClearDepthStencilValue*>(&depthStencilAttachment->ClearValue);

	    stencilAttachment	      = depthAttachment;
	    stencilAttachment.loadOp  = VulkanUtilities::Translate(depthStencilAttachment->StencilLoadOp);
	    stencilAttachment.storeOp = VulkanUtilities::Translate(depthStencilAttachment->StencilStoreOp);
	}

	VkRenderingInfoKHR renderingInfo = { VK_STRUCTURE_TYPE_RENDERING_INFO_KHR };
	{
	    renderingInfo.renderArea	       = *reinterpret_cast<const VkRect2D*>(&renderArea);
	    renderingInfo.layerCount	       = 1;
	    renderingInfo.viewMask	       = 0;
	    renderingInfo.colorAttachmentCount = colorAttachmentCount;
	    renderingInfo.pColorAttachments    = colorAttachmentsVk.data();
	    renderingInfo.pDepthAttachment     = depthStencilAttachment ? &depthAttachment : nullptr;
	    renderingInfo.pStencilAttachment   = depthStencilAttachment ? &stencilAttachment : nullptr;
	}

	vkCmdBeginRenderingKHR(m_commandBuffer, &renderingInfo);
    }
    else
    {
	VkRenderPass renderPass	  = VK_NULL_HANDLE;
	VkFramebuffer frameBuffer = VK_NULL_HANDLE;

	VkClearValue clearValues[9]								  = {};
	VkImageView attachmentViews[9]								  = {};
	VulkanRenderPassDescription::ColorAttachmentDescription colorAttachmentDescs[8]		  = {};
	VulkanRenderPassDescription::DepthStencilAttachmentDescription depthStencilAttachmentDesc = {};
	VulkanFrameBufferDescription::AttachmentInfo frameBufferAttachmentInfo[9]		  = {};

	uint32_t attachmentCount   = 0;
	uint32_t frameBufferWidth  = -1;
	uint32_t frameBufferHeight = -1;

	for(uint32_t i = 0; i < colorAttachmentCount; i++)
	{
	    const auto& attachment = colorAttachments[i];
	    const auto* image	   = attachment.ImageView->GetImage();
	    const auto& imageDesc  = image->GetDescription();

	    frameBufferWidth  = MIN(frameBufferWidth, imageDesc.Width);
	    frameBufferHeight = MIN(frameBufferHeight, imageDesc.Height);

	    attachmentViews[i]	 = static_cast<VkImageView>(attachment.ImageView->GetNativeHandle());
	    clearValues[i].color = *reinterpret_cast<const VkClearColorValue*>(&attachment.ClearValue);

	    auto& attachmentDesc = colorAttachmentDescs[i];
	    {
		attachmentDesc	       = {};
		attachmentDesc.Format  = VulkanUtilities::Translate(imageDesc.Format);
		attachmentDesc.Samples = static_cast<VkSampleCountFlagBits>(imageDesc.Samples);
		attachmentDesc.LoadOp  = VulkanUtilities::Translate(attachment.LoadOp);
		attachmentDesc.StoreOp = VulkanUtilities::Translate(attachment.StoreOp);
	    }

	    const auto& viewDesc = attachment.ImageView->GetDescription();

	    auto& frameBufferInfo = frameBufferAttachmentInfo[i];
	    {
		frameBufferInfo		   = {};
		frameBufferInfo.Flags	   = VulkanUtilities::Translate(imageDesc.CreateFlags);
		frameBufferInfo.Usage	   = VulkanUtilities::Translate(imageDesc.UsageFlags);
		frameBufferInfo.Width	   = imageDesc.Width;
		frameBufferInfo.Height	   = imageDesc.Height;
		frameBufferInfo.LayerCount = viewDesc.LayerCount;
		frameBufferInfo.Format	   = attachmentDesc.Format;
	    }

	    attachmentCount++;
	}

	if(depthStencilAttachment)
	{
	    const auto& attachment = *depthStencilAttachment;
	    const auto* image	   = attachment.ImageView->GetImage();
	    const auto& imageDesc  = image->GetDescription();

	    frameBufferWidth  = MIN(frameBufferWidth, imageDesc.Width);
	    frameBufferHeight = MIN(frameBufferHeight, imageDesc.Height);

	    attachmentViews[attachmentCount]	      = (VkImageView) attachment.ImageView->GetNativeHandle();
	    clearValues[attachmentCount].depthStencil = *reinterpret_cast<const VkClearDepthStencilValue*>(&attachment.ClearValue);

	    auto& attachmentDesc = depthStencilAttachmentDesc;
	    {
		attachmentDesc		      = {};
		attachmentDesc.Format	      = VulkanUtilities::Translate(imageDesc.Format);
		attachmentDesc.Samples	      = static_cast<VkSampleCountFlagBits>(imageDesc.Samples);
		attachmentDesc.LoadOp	      = VulkanUtilities::Translate(attachment.LoadOp);
		attachmentDesc.StoreOp	      = VulkanUtilities::Translate(attachment.StoreOp);
		attachmentDesc.StencilLoadOp  = VulkanUtilities::Translate(attachment.StencilLoadOp);
		attachmentDesc.StencilStoreOp = VulkanUtilities::Translate(attachment.StencilStoreOp);
		attachmentDesc.Layout	      = attachment.ReadOnly ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	    }

	    const auto& viewDesc = attachment.ImageView->GetDescription();

	    auto& frameBufferInfo = frameBufferAttachmentInfo[attachmentCount];
	    {
		frameBufferInfo		   = {};
		frameBufferInfo.Flags	   = VulkanUtilities::Translate(imageDesc.CreateFlags);
		frameBufferInfo.Usage	   = VulkanUtilities::Translate(imageDesc.UsageFlags);
		frameBufferInfo.Width	   = imageDesc.Width;
		frameBufferInfo.Height	   = imageDesc.Height;
		frameBufferInfo.LayerCount = viewDesc.LayerCount;
		frameBufferInfo.Format	   = attachmentDesc.Format;
	    }
	    attachmentCount++;
	}

	// Get RenderPass
	{
	    VulkanRenderPassDescription renderPassDesc;
	    renderPassDesc.SetColorAttachment(colorAttachmentCount, colorAttachmentDescs);
	    if(depthStencilAttachment)
	    {
		renderPassDesc.SetDepthStencilAttachment(depthStencilAttachmentDesc);
	    }
	    renderPassDesc.Finalize();

	    renderPass = m_adapter->GetRenderPass(renderPassDesc);
	}

	// Get FrameBuffer
	{
	    if(frameBufferWidth == -1 || frameBufferHeight == -1)
	    {
		frameBufferWidth  = renderArea.Offset.X + renderArea.Extent.Width;
		frameBufferHeight = renderArea.Offset.Y + renderArea.Extent.Height;
	    }

	    VulkanFrameBufferDescription frameBufferDesc;
	    frameBufferDesc.SetRenderPass(renderPass);
	    frameBufferDesc.SetAttachment(attachmentCount, frameBufferAttachmentInfo);
	    frameBufferDesc.SetExtent(frameBufferWidth, frameBufferHeight, 1);
	    frameBufferDesc.Finalize();

	    frameBuffer = m_adapter->GetFrameBuffer(frameBufferDesc);
	}

	VkRenderPassAttachmentBeginInfo attachmentBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO };
	{
	    attachmentBeginInfo.attachmentCount = attachmentCount;
	    attachmentBeginInfo.pAttachments	= attachmentViews;
	}

	VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	{
	    renderPassBeginInfo.pNext		= &attachmentBeginInfo;
	    renderPassBeginInfo.renderPass	= renderPass;
	    renderPassBeginInfo.framebuffer	= frameBuffer;
	    renderPassBeginInfo.renderArea	= *reinterpret_cast<const VkRect2D*>(&renderArea);
	    renderPassBeginInfo.clearValueCount = attachmentCount;
	    renderPassBeginInfo.pClearValues	= clearValues;
	}

	vkCmdBeginRenderPass(m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }
}

void VulkanCommand::EndRenderPass()
{
    if(m_adapter->IsDynamicRenderingExtensionSupported())
    {
	vkCmdEndRenderingKHR(m_commandBuffer);
    }
    else
    {
	vkCmdEndRenderPass(m_commandBuffer);
    }
}
