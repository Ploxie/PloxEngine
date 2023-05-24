//
// Created by Ploxie on 2023-05-17.
//

#pragma once

#include "rendering/types/Barrier.h"
#include "rendering/types/Format.h"
#include "rendering/types/GraphicsPipeline.h"
namespace RenderUtilities
{
    bool IsDepthFormat(Format format);
    bool IsStencilFormat(Format format);

    uint32_t GetUsageFlags(ResourceState state, bool isImage);

    Barrier ImageBarrier(const Image* image, PipelineStageFlags stagesBefore, PipelineStageFlags stagesAfter, ResourceState stateBefore, ResourceState stateAfter, const ImageSubresourceRange& subresourceRange = { 0, 1, 0, 1 });
} // namespace RenderUtilities

class GraphicsPipelineBuilder
{
public:
    static const PipelineColorBlendAttachmentState s_defaultBlendAttachment;

    explicit GraphicsPipelineBuilder(GraphicsPipelineCreateInfo& createInfo);
    void SetVertexShader(const char* path);
    void SetTessControlShader(const char* path);
    void SetTessEvalShader(const char* path);
    void SetGeometryShader(const char* path);
    void SetFragmentShader(const char* path);
    void SetVertexBindingDescriptions(size_t count, const VertexInputBindingDescription* bindingDescs);
    void SetVertexBindingDescription(const VertexInputBindingDescription& bindingDesc);
    void SetVertexAttributeDescriptions(size_t count, const VertexInputAttributeDescription* attributeDescs);
    void SetVertexAttributeDescription(const VertexInputAttributeDescription& attributeDesc);
    void SetInputAssemblyState(PrimitiveTopology topology, bool primitiveRestartEnable);
    void SetTesselationState(uint32_t patchControlPoints);
    void SetViewportScissors(size_t count, const Viewport* viewports, const Rect* scissors);
    void SetViewportScissor(const Viewport& viewport, const Rect& scissor);
    void SetDepthClampEnable(bool depthClampEnable);
    void SetRasterizerDiscardEnable(bool rasterizerDiscardEnable);
    void SetPolygonModeCullMode(PolygonMode polygonMode, CullModeFlags cullMode, FrontFace frontFace);
    void SetDepthBias(bool enable, float constantFactor, float clamp, float slopeFactor);
    void SetLineWidth(float lineWidth);
    void SetMultisampleState(SampleCount rasterizationSamples, bool sampleShadingEnable, float minSampleShading, uint32_t sampleMask, bool alphaToCoverageEnable, bool alphaToOneEnable);
    void SetDepthTest(bool depthTestEnable, bool depthWriteEnable, CompareOp depthCompareOp);
    void SetStencilTest(bool stencilTestEnable, const StencilOpState& front, const StencilOpState& back);
    void SetDepthBoundsTest(bool depthBoundsTestEnable, float minDepthBounds, float maxDepthBounds);
    void SetBlendStateLogicOp(bool logicOpEnable, LogicOp logicOp);
    void SetBlendConstants(float blendConst0, float blendConst1, float blendConst2, float blendConst3);
    void SetColorBlendAttachments(size_t count, const PipelineColorBlendAttachmentState* colorBlendAttachments);
    void SetColorBlendAttachment(const PipelineColorBlendAttachmentState& colorBlendAttachment);
    void SetDynamicState(DynamicStateFlags dynamicStateFlags);
    void SetColorAttachmentFormats(uint32_t count, Format* formats);
    void SetColorAttachmentFormat(Format format);
    void SetDepthStencilAttachmentFormat(Format format);
    void SetPipelineLayoutDescription(
	uint32_t setLayoutCount,
	const DescriptorSetLayoutDeclaration* setLayoutDeclarations,
	uint32_t pushConstRange,
	ShaderStageFlags pushConstStageFlags,
	uint32_t staticSamplerCount,
	const StaticSamplerDescription* staticSamplerDescriptions,
	uint32_t staticSamplerSet);

private:
    GraphicsPipelineCreateInfo& m_createInfo;
};