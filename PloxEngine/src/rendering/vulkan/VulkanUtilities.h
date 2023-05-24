//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "rendering/types/Barrier.h"
#include "rendering/types/Buffer.h"
#include "rendering/types/Command.h"
#include "rendering/types/Format.h"
#include "rendering/types/GraphicsPipeline.h"
#include "rendering/types/ImageView.h"
#include "vulkan/vulkan.h"

namespace VulkanUtilities
{
    VkResult checkResult(VkResult result, const char *errorMsg = nullptr, bool exitOnError = true);

    void Translate(DynamicStateFlags flags, uint32_t &stateCount, VkDynamicState states[9]);
    VkShaderStageFlags Translate(ShaderStageFlags flags);
    VkColorComponentFlags Translate(ColorComponentFlags flags);
    VkBlendOp Translate(BlendOp blendOp);
    VkBlendFactor Translate(BlendFactor blendFactor);
    VkLogicOp Translate(LogicOp logicOp);
    VkCompareOp Translate(CompareOp compareOp);
    VkStencilOp Translate(StencilOp stencilOp);
    VkFrontFace Translate(FrontFace frontFace);
    VkCullModeFlags Translate(CullModeFlags flags);
    VkPolygonMode Translate(PolygonMode polygonMode);
    VkPrimitiveTopology Translate(PrimitiveTopology primitiveTopology);
    VkVertexInputRate Translate(VertexInputRate inputRate);
    VkFormat Translate(Format format);
    Format Translate(VkFormat format);
    VkImageType Translate(ImageType imageType);
    VkImageViewType Translate(ImageViewType imageViewType);
    VkBufferCreateFlags Translate(BufferCreateFlags flags);
    VkBufferUsageFlags Translate(BufferUsageFlags flags);
    VkComponentSwizzle Translate(ComponentSwizzle swizzle);
    VkFilter Translate(Filter filter);
    VkImageAspectFlags GetImageAspectMask(VkFormat format);
    VkSamplerMipmapMode Translate(SamplerMipmapMode mipmapMode);
    VkSamplerAddressMode Translate(SamplerAddressMode addressMode);
    VkBorderColor Translate(BorderColor borderColor);
    VkStencilFaceFlags Translate(StencilFaceFlags flags);
    VkAttachmentLoadOp Translate(AttachmentLoadOp loadOp);
    VkAttachmentStoreOp Translate(AttachmentStoreOp storeOp);
    VkImageCreateFlags Translate(ImageCreateFlags flags);
    VkMemoryPropertyFlags Translate(MemoryPropertyFlags flags);
    VkPipelineStageFlags Translate(PipelineStageFlags flags);
    VkImageUsageFlags Translate(ImageUsageFlags flags);
    VkDescriptorBindingFlags Translate(DescriptorBindingFlags flags);

} // namespace VulkanUtilities