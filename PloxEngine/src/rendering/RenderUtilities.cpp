//
// Created by Ploxie on 2023-05-17.
//
#include "RenderUtilities.h"

bool RenderUtilities::IsDepthFormat(Format format)
{
    switch(format)
    {
	case Format::D16_UNORM:
	case Format::X8_D24_UNORM_PACK32:
	case Format::D32_SFLOAT:
	case Format::D16_UNORM_S8_UINT:
	case Format::D24_UNORM_S8_UINT:
	case Format::D32_SFLOAT_S8_UINT:
	    return true;
	default:
	    return false;
    }
}

bool RenderUtilities::IsStencilFormat(Format format)
{
    switch(format)
    {
	case Format::S8_UINT:
	case Format::D16_UNORM_S8_UINT:
	case Format::D24_UNORM_S8_UINT:
	case Format::D32_SFLOAT_S8_UINT:
	    return true;
	default:
	    return false;
    }
}

Barrier RenderUtilities::ImageBarrier(const Image *image, PipelineStageFlags stagesBefore, PipelineStageFlags stagesAfter, ResourceState stateBefore, ResourceState stateAfter, const ImageSubresourceRange &subresourceRange)
{
    return { image, nullptr, stagesBefore, stagesAfter, stateBefore, stateAfter, nullptr, nullptr, subresourceRange, {} };
}

GraphicsPipelineBuilder::GraphicsPipelineBuilder(GraphicsPipelineCreateInfo &createInfo)
    : m_createInfo(createInfo)
{
    memset(&m_createInfo, 0, sizeof(m_createInfo));
    m_createInfo.ViewportState.ViewportCount	       = 1;
    m_createInfo.ViewportState.Viewports[0]	       = { 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
    m_createInfo.ViewportState.Scissors[0]	       = { { 0, 0 }, { 1, 1 } };
    m_createInfo.InputAssemblyState.PrimitiveTopology  = PrimitiveTopology::TRIANGLE_LIST;
    m_createInfo.MultiSampleState.RasterizationSamples = SampleCount::_1;
    m_createInfo.MultiSampleState.SampleMask	       = 0xFFFFFFFF;
    m_createInfo.RasterizationState.LineWidth	       = 1.0f;
}

void GraphicsPipelineBuilder::SetVertexShader(const char *path)
{
    strcpy_s(m_createInfo.VertexShader.Path, path);
}

void GraphicsPipelineBuilder::SetTessControlShader(const char *path)
{
    strcpy_s(m_createInfo.HullShader.Path, path);
}

void GraphicsPipelineBuilder::SetTessEvalShader(const char *path)
{
    strcpy_s(m_createInfo.DomainShader.Path, path);
}

void GraphicsPipelineBuilder::SetGeometryShader(const char *path)
{
    strcpy_s(m_createInfo.GeometryShader.Path, path);
}

void GraphicsPipelineBuilder::SetFragmentShader(const char *path)
{
    strcpy_s(m_createInfo.PixelShader.Path, path);
}

void GraphicsPipelineBuilder::SetVertexBindingDescriptions(size_t count, const VertexInputBindingDescription *bindingDescs)
{
    ASSERT(count < VertexInputState::MAX_VERTEX_BINDING_DESCRIPTIONS);
    m_createInfo.VertexInputState.VertexBindingDescriptionCount = static_cast<uint32_t>(count);
    for(size_t i = 0; i < count; ++i)
    {
	m_createInfo.VertexInputState.VertexBindingDescriptions[i] = bindingDescs[i];
    }
}

void GraphicsPipelineBuilder::SetVertexBindingDescription(const VertexInputBindingDescription &bindingDesc)
{
    m_createInfo.VertexInputState.VertexBindingDescriptionCount = 1;
    m_createInfo.VertexInputState.VertexBindingDescriptions[0]	= bindingDesc;
}

void GraphicsPipelineBuilder::SetVertexAttributeDescriptions(size_t count, const VertexInputAttributeDescription *attributeDescs)
{
    ASSERT(count < VertexInputState::MAX_VERTEX_ATTRIBUTE_DESCRIPTIONS);
    m_createInfo.VertexInputState.VertexAttributeDescriptionCount = static_cast<uint32_t>(count);
    for(size_t i = 0; i < count; ++i)
    {
	m_createInfo.VertexInputState.VertexAttributeDescriptions[i] = attributeDescs[i];
    }
}

void GraphicsPipelineBuilder::SetVertexAttributeDescription(const VertexInputAttributeDescription &attributeDesc)
{
    m_createInfo.VertexInputState.VertexAttributeDescriptionCount = 1;
    m_createInfo.VertexInputState.VertexAttributeDescriptions[0]  = attributeDesc;
}

void GraphicsPipelineBuilder::SetInputAssemblyState(PrimitiveTopology topology, bool primitiveRestartEnable)
{
    m_createInfo.InputAssemblyState.PrimitiveTopology	   = topology;
    m_createInfo.InputAssemblyState.PrimitiveRestartEnable = primitiveRestartEnable;
}

void GraphicsPipelineBuilder::SetTesselationState(uint32_t patchControlPoints)
{
    m_createInfo.TesselationState.PatchControlPoints = patchControlPoints;
}

void GraphicsPipelineBuilder::SetViewportScissors(size_t count, const Viewport *viewports, const Rect *scissors)
{
    ASSERT(count < ViewportState::MAX_VIEWPORTS);
    m_createInfo.ViewportState.ViewportCount = static_cast<uint32_t>(count);
    for(size_t i = 0; i < count; ++i)
    {
	m_createInfo.ViewportState.Viewports[i] = viewports[i];
	m_createInfo.ViewportState.Scissors[i]	= scissors[i];
    }
}

void GraphicsPipelineBuilder::SetViewportScissor(const Viewport &viewport, const Rect &scissor)
{
    m_createInfo.ViewportState.ViewportCount = 1;
    m_createInfo.ViewportState.Viewports[0]  = viewport;
    m_createInfo.ViewportState.Scissors[0]   = scissor;
}

void GraphicsPipelineBuilder::SetDepthClampEnable(bool depthClampEnable)
{
    m_createInfo.RasterizationState.DepthClampEnable = depthClampEnable;
}

void GraphicsPipelineBuilder::SetRasterizerDiscardEnable(bool rasterizerDiscardEnable)
{
    m_createInfo.RasterizationState.RasterizerDiscardEnable = rasterizerDiscardEnable;
}

void GraphicsPipelineBuilder::SetPolygonModeCullMode(PolygonMode polygonMode, CullModeFlags cullMode, FrontFace frontFace)
{
    m_createInfo.RasterizationState.PolygonMode = polygonMode;
    m_createInfo.RasterizationState.CullMode	= cullMode;
    m_createInfo.RasterizationState.FrontFace	= frontFace;
}

void GraphicsPipelineBuilder::SetDepthBias(bool enable, float constantFactor, float clamp, float slopeFactor)
{
    m_createInfo.RasterizationState.DepthBiasEnable	    = enable;
    m_createInfo.RasterizationState.DepthBiasConstantFactor = constantFactor;
    m_createInfo.RasterizationState.DepthBiasClamp	    = clamp;
    m_createInfo.RasterizationState.DepthBiasSlopeFactor    = slopeFactor;
}

void GraphicsPipelineBuilder::SetLineWidth(float lineWidth)
{
    m_createInfo.RasterizationState.LineWidth = lineWidth;
}

void GraphicsPipelineBuilder::SetMultisampleState(SampleCount rasterizationSamples, bool sampleShadingEnable, float minSampleShading, uint32_t sampleMask, bool alphaToCoverageEnable, bool alphaToOneEnable)
{
    m_createInfo.MultiSampleState.RasterizationSamples	= rasterizationSamples;
    m_createInfo.MultiSampleState.SampleShadingEnable	= sampleShadingEnable;
    m_createInfo.MultiSampleState.MinSampleShading	= minSampleShading;
    m_createInfo.MultiSampleState.SampleMask		= sampleMask;
    m_createInfo.MultiSampleState.AlphaToCoverageEnable = alphaToCoverageEnable;
    m_createInfo.MultiSampleState.AlphaToOneEnable	= alphaToOneEnable;
}

void GraphicsPipelineBuilder::SetDepthTest(bool depthTestEnable, bool depthWriteEnable, CompareOp depthCompareOp)
{
    m_createInfo.DepthStencilState.DepthTestEnable  = depthTestEnable;
    m_createInfo.DepthStencilState.DepthWriteEnable = depthWriteEnable;
    m_createInfo.DepthStencilState.DepthCompareOp   = depthCompareOp;
}

void GraphicsPipelineBuilder::SetStencilTest(bool stencilTestEnable, const StencilOpState &front, const StencilOpState &back)
{
    m_createInfo.DepthStencilState.StencilTestEnable = stencilTestEnable;
    m_createInfo.DepthStencilState.Front	     = front;
    m_createInfo.DepthStencilState.Back		     = back;
}

void GraphicsPipelineBuilder::SetDepthBoundsTest(bool depthBoundsTestEnable, float minDepthBounds, float maxDepthBounds)
{
    m_createInfo.DepthStencilState.DepthBoundsTestEnable = depthBoundsTestEnable;
    m_createInfo.DepthStencilState.MinDepthBounds	 = minDepthBounds;
    m_createInfo.DepthStencilState.MaxDepthBounds	 = maxDepthBounds;
}

void GraphicsPipelineBuilder::SetBlendStateLogicOp(bool logicOpEnable, LogicOp logicOp)
{
    m_createInfo.BlendState.LogicOpEnable = logicOpEnable;
    m_createInfo.BlendState.LogicOp	  = logicOp;
}

void GraphicsPipelineBuilder::SetBlendConstants(float blendConst0, float blendConst1, float blendConst2, float blendConst3)
{
    m_createInfo.BlendState.BlendConstants[0] = blendConst0;
    m_createInfo.BlendState.BlendConstants[1] = blendConst1;
    m_createInfo.BlendState.BlendConstants[2] = blendConst2;
    m_createInfo.BlendState.BlendConstants[3] = blendConst3;
}

void GraphicsPipelineBuilder::SetColorBlendAttachments(size_t count, const PipelineColorBlendAttachmentState *colorBlendAttachments)
{
    ASSERT(count < 8);
    m_createInfo.BlendState.AttachmentCount = static_cast<uint32_t>(count);
    for(size_t i = 0; i < count; ++i)
    {
	m_createInfo.BlendState.Attachments[i] = colorBlendAttachments[i];
    }
}

void GraphicsPipelineBuilder::SetColorBlendAttachment(const PipelineColorBlendAttachmentState &colorBlendAttachment)
{
    m_createInfo.BlendState.AttachmentCount = 1;
    m_createInfo.BlendState.Attachments[0]  = colorBlendAttachment;
}

void GraphicsPipelineBuilder::SetDynamicState(DynamicStateFlags dynamicStateFlags)
{
    m_createInfo.DynamicStateFlags = dynamicStateFlags;
}

void GraphicsPipelineBuilder::SetColorAttachmentFormats(uint32_t count, Format *formats)
{
    ASSERT(count < 8);
    m_createInfo.AttachmentFormats.ColorAttachmentCount = count;
    for(size_t i = 0; i < count; ++i)
    {
	m_createInfo.AttachmentFormats.ColorAttachmentFormats[i] = formats[i];
    }
}

void GraphicsPipelineBuilder::SetColorAttachmentFormat(Format format)
{
    m_createInfo.AttachmentFormats.ColorAttachmentCount	     = 1;
    m_createInfo.AttachmentFormats.ColorAttachmentFormats[0] = format;
}

void GraphicsPipelineBuilder::SetDepthStencilAttachmentFormat(Format format)
{
    m_createInfo.AttachmentFormats.DepthStencilFormat = format;
}

void GraphicsPipelineBuilder::SetPipelineLayoutDescription(
    uint32_t setLayoutCount,
    const DescriptorSetLayoutDeclaration *setLayoutDeclarations,
    uint32_t pushConstRange,
    ShaderStageFlags pushConstStageFlags,
    uint32_t staticSamplerCount,
    const StaticSamplerDescription *staticSamplerDescriptions,
    uint32_t staticSamplerSet)
{
    m_createInfo.LayoutCreateInfo.DescriptorSetLayoutCount = setLayoutCount;
    for(size_t i = 0; i < setLayoutCount; ++i)
    {
	m_createInfo.LayoutCreateInfo.DescriptorSetLayoutDeclarations[i] = setLayoutDeclarations[i];
    }
    m_createInfo.LayoutCreateInfo.PushConstRange	    = pushConstRange;
    m_createInfo.LayoutCreateInfo.PushConstStageFlags	    = pushConstStageFlags;
    m_createInfo.LayoutCreateInfo.StaticSamplerCount	    = staticSamplerCount;
    m_createInfo.LayoutCreateInfo.StaticSamplerDescriptions = staticSamplerDescriptions;
    m_createInfo.LayoutCreateInfo.StaticSamplerSet	    = staticSamplerSet;
}
