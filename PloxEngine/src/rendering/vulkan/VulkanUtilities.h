//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "rendering/types/Format.h"
#include "rendering/types/GraphicsPipeline.h"
#include "vulkan/vulkan.h"

namespace VulkanUtilities
{
    VkResult checkResult(VkResult result, const char *errorMsg = nullptr, bool exitOnError = true);

    void Translate(DynamicStateFlags flags, uint32_t &stateCount, VkDynamicState states[9]);
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

} // namespace VulkanUtilities