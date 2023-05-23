//
// Created by Ploxie on 2023-05-17.
//
#include "VulkanGraphicsPipeline.h"
#include "platform/Platform.h"
#include "volk.h"
#include "VulkanDescriptorSet.h"
#include "VulkanGraphicsAdapter.h"
#include "VulkanRenderPassDescription.h"
#include "VulkanUtilities.h"

static void CreateShaderStage(VkDevice device, const ShaderStageCreateInfo& stageDesc, VkShaderStageFlagBits stageFlag, VkShaderModule& shaderModule, VkPipelineShaderStageCreateInfo& stageCreateInfo);
static void CreatePipelineLayout(VkDevice device, const PipelineLayoutCreateInfo& layoutCreateInfo, VkPipelineLayout& pipelineLayout, VkDescriptorSetLayout& staticSamplerDescriptorSetLayout, VkDescriptorPool& staticDescriptorPool, VkDescriptorSet& staticSamplerDescriptorSet, eastl::fixed_vector<VkSampler, 16>& staticSamplers);

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanGraphicsAdapter* adapter, const GraphicsPipelineCreateInfo& createInfo)
    : m_pipeline(VK_NULL_HANDLE), m_pipelineLayout(VK_NULL_HANDLE), m_adapter(adapter)
{
    VkDevice device				    = adapter->GetDevice();
    uint32_t stageCount				    = 0;
    VkShaderModule shaderModules[5]		    = {};
    VkPipelineShaderStageCreateInfo shaderStages[5] = {};
    VkRenderPass renderPass			    = VK_NULL_HANDLE;

    // Create shaders
    {
	if(createInfo.VertexShader.Path[0])
	{
	    CreateShaderStage(device, createInfo.VertexShader, VK_SHADER_STAGE_VERTEX_BIT, shaderModules[stageCount], shaderStages[stageCount]);
	    stageCount++;
	}

	if(createInfo.HullShader.Path[0])
	{
	    CreateShaderStage(device, createInfo.HullShader, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, shaderModules[stageCount], shaderStages[stageCount]);
	    stageCount++;
	}

	if(createInfo.DomainShader.Path[0])
	{
	    CreateShaderStage(device, createInfo.DomainShader, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, shaderModules[stageCount], shaderStages[stageCount]);
	    stageCount++;
	}

	if(createInfo.GeometryShader.Path[0])
	{
	    CreateShaderStage(device, createInfo.GeometryShader, VK_SHADER_STAGE_GEOMETRY_BIT, shaderModules[stageCount], shaderStages[stageCount]);
	    stageCount++;
	}

	if(createInfo.PixelShader.Path[0])
	{
	    CreateShaderStage(device, createInfo.PixelShader, VK_SHADER_STAGE_FRAGMENT_BIT, shaderModules[stageCount], shaderStages[stageCount]);
	    stageCount++;
	}
    }

    VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR };
    VkFormat colorAttachmentFormats[8];

    const bool dynamicRenderingEnabled = adapter->IsDynamicRenderingExtensionSupported();
    if(dynamicRenderingEnabled)
    {
	for(size_t i = 0; i < createInfo.AttachmentFormats.ColorAttachmentCount; i++)
	{
	    colorAttachmentFormats[i] = VulkanUtilities::Translate(createInfo.AttachmentFormats.ColorAttachmentFormats[i]);
	}

	pipelineRenderingCreateInfo.viewMask		    = 0;
	pipelineRenderingCreateInfo.colorAttachmentCount    = createInfo.AttachmentFormats.ColorAttachmentCount;
	pipelineRenderingCreateInfo.pColorAttachmentFormats = colorAttachmentFormats;
	pipelineRenderingCreateInfo.depthAttachmentFormat   = VulkanUtilities::Translate(createInfo.AttachmentFormats.DepthStencilFormat);
	pipelineRenderingCreateInfo.stencilAttachmentFormat = pipelineRenderingCreateInfo.stencilAttachmentFormat;
    }
    else
    {
	VulkanRenderPassDescription::ColorAttachmentDescription colorAttachments[8];
	VulkanRenderPassDescription::DepthStencilAttachmentDescription depthStencilAttachment;

	uint32_t attachmentCount = 0;

	auto samples = static_cast<VkSampleCountFlagBits>(createInfo.MultiSampleState.RasterizationSamples);

	for(uint32_t i = 0; i < createInfo.AttachmentFormats.ColorAttachmentCount; i++)
	{
	    auto& attachmentDesc = colorAttachments[i];
	    attachmentDesc	 = {};
	    {
		attachmentDesc.Format  = VulkanUtilities::Translate(createInfo.AttachmentFormats.ColorAttachmentFormats[i]);
		attachmentDesc.Samples = samples;
		attachmentDesc.LoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;
		attachmentDesc.StoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	    }

	    attachmentCount++;
	}

	if(createInfo.DepthStencilState.DepthTestEnable)
	{
	    auto& attachmentDesc = depthStencilAttachment;
	    attachmentDesc	 = {};
	    {
		attachmentDesc.Format	      = VulkanUtilities::Translate(createInfo.AttachmentFormats.DepthStencilFormat);
		attachmentDesc.Samples	      = samples;
		attachmentDesc.LoadOp	      = VK_ATTACHMENT_LOAD_OP_LOAD;
		attachmentDesc.StoreOp	      = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDesc.StencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDesc.StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDesc.Layout	      = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	    }
	    attachmentCount++;
	}

	// Get RenderPass
	{
	    VulkanRenderPassDescription renderPassDescription;
	    renderPassDescription.SetColorAttachment(createInfo.AttachmentFormats.ColorAttachmentCount, colorAttachments);
	    if(createInfo.DepthStencilState.DepthTestEnable)
	    {
		renderPassDescription.SetDepthStencilAttachment(depthStencilAttachment);
	    }
	    renderPassDescription.Finalize();

	    renderPass = adapter->GetRenderPass(renderPassDescription);
	}
    }

    // Create Pipeline layout
    m_staticSamplerDescriptorSetIndex = createInfo.LayoutCreateInfo.StaticSamplerSet;
    CreatePipelineLayout(device, createInfo.LayoutCreateInfo, m_pipelineLayout, m_staticSamplerDescriptorSetLayout, m_staticSamplerDescriptorPool, m_staticSamplerDescriptorSet, m_staticSamplers);

    VkPipelineVertexInputStateCreateInfo vertexInputState = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    VkVertexInputBindingDescription vertexBindingDescriptions[VertexInputState::MAX_VERTEX_BINDING_DESCRIPTIONS];
    VkVertexInputAttributeDescription vertexAttributeDescriptions[VertexInputState::MAX_VERTEX_ATTRIBUTE_DESCRIPTIONS];
    {
	vertexInputState.vertexBindingDescriptionCount	 = createInfo.VertexInputState.VertexBindingDescriptionCount;
	vertexInputState.pVertexBindingDescriptions	 = vertexBindingDescriptions;
	vertexInputState.vertexAttributeDescriptionCount = createInfo.VertexInputState.VertexAttributeDescriptionCount;
	vertexInputState.pVertexAttributeDescriptions	 = vertexAttributeDescriptions;

	for(size_t i = 0; i < createInfo.VertexInputState.VertexBindingDescriptionCount; i++)
	{
	    auto& binding	    = vertexBindingDescriptions[i];
	    const auto& description = createInfo.VertexInputState.VertexBindingDescriptions[i];
	    binding.binding	    = description.Binding;
	    binding.stride	    = description.Stride;
	    binding.inputRate	    = VulkanUtilities::Translate(description.InputRate);
	}

	for(size_t i = 0; i < createInfo.VertexInputState.VertexAttributeDescriptionCount; i++)
	{
	    auto& attribute	    = vertexAttributeDescriptions[i];
	    const auto& description = createInfo.VertexInputState.VertexAttributeDescriptions[i];
	    attribute.location	    = description.Location;
	    attribute.binding	    = description.Binding;
	    attribute.format	    = VulkanUtilities::Translate(description.Format);
	    attribute.offset	    = description.Offset;
	}
    }

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    {
	inputAssemblyState.topology		  = VulkanUtilities::Translate(createInfo.InputAssemblyState.PrimitiveTopology);
	inputAssemblyState.primitiveRestartEnable = createInfo.InputAssemblyState.PrimitiveRestartEnable;
    }

    VkPipelineTessellationStateCreateInfo tessellationState = { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
    {
	tessellationState.patchControlPoints = createInfo.TesselationState.PatchControlPoints;
    }

    VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    {
	viewportState.viewportCount = createInfo.ViewportState.ViewportCount;
	viewportState.pViewports    = reinterpret_cast<const VkViewport*>(createInfo.ViewportState.Viewports);
	viewportState.scissorCount  = createInfo.ViewportState.ViewportCount;
	viewportState.pScissors	    = reinterpret_cast<const VkRect2D*>(createInfo.ViewportState.Scissors);
    }

    VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    {
	rasterizationState.depthClampEnable	   = createInfo.RasterizationState.DepthClampEnable;
	rasterizationState.rasterizerDiscardEnable = createInfo.RasterizationState.RasterizerDiscardEnable;
	rasterizationState.polygonMode		   = VulkanUtilities::Translate(createInfo.RasterizationState.PolygonMode);
	rasterizationState.cullMode		   = VulkanUtilities::Translate(createInfo.RasterizationState.CullMode);
	rasterizationState.frontFace		   = VulkanUtilities::Translate(createInfo.RasterizationState.FrontFace);
	rasterizationState.depthBiasEnable	   = createInfo.RasterizationState.DepthBiasEnable;
	rasterizationState.depthBiasConstantFactor = createInfo.RasterizationState.DepthBiasConstantFactor;
	rasterizationState.depthBiasClamp	   = createInfo.RasterizationState.DepthBiasClamp;
	rasterizationState.depthBiasSlopeFactor	   = createInfo.RasterizationState.DepthBiasSlopeFactor;
	rasterizationState.lineWidth		   = createInfo.RasterizationState.LineWidth;
    }

    VkPipelineMultisampleStateCreateInfo multisamplingState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    {
	multisamplingState.rasterizationSamples	 = static_cast<VkSampleCountFlagBits>(createInfo.MultiSampleState.RasterizationSamples);
	multisamplingState.sampleShadingEnable	 = createInfo.MultiSampleState.SampleShadingEnable;
	multisamplingState.minSampleShading	 = createInfo.MultiSampleState.MinSampleShading;
	multisamplingState.pSampleMask		 = &createInfo.MultiSampleState.SampleMask;
	multisamplingState.alphaToCoverageEnable = createInfo.MultiSampleState.AlphaToCoverageEnable;
	multisamplingState.alphaToOneEnable	 = createInfo.MultiSampleState.AlphaToOneEnable;
    }

    VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
    {
	auto translateStencilOpState = [](const StencilOpState& state)
	{
	    VkStencilOpState result = {};
	    result.failOp	    = VulkanUtilities::Translate(state.FailOp);
	    result.passOp	    = VulkanUtilities::Translate(state.PassOp);
	    result.depthFailOp	    = VulkanUtilities::Translate(state.DepthFailOp);
	    result.compareOp	    = VulkanUtilities::Translate(state.CompareOp);
	    result.compareMask	    = state.CompareMask;
	    result.writeMask	    = state.WriteMask;
	    result.reference	    = state.Reference;
	    return result;
	};

	depthStencilState.depthTestEnable	= createInfo.DepthStencilState.DepthTestEnable;
	depthStencilState.depthWriteEnable	= createInfo.DepthStencilState.DepthWriteEnable;
	depthStencilState.depthCompareOp	= VulkanUtilities::Translate(createInfo.DepthStencilState.DepthCompareOp);
	depthStencilState.depthBoundsTestEnable = createInfo.DepthStencilState.DepthBoundsTestEnable;
	depthStencilState.stencilTestEnable	= createInfo.DepthStencilState.StencilTestEnable;
	depthStencilState.front			= translateStencilOpState(createInfo.DepthStencilState.Front);
	depthStencilState.back			= translateStencilOpState(createInfo.DepthStencilState.Back);
	depthStencilState.minDepthBounds	= createInfo.DepthStencilState.MinDepthBounds;
	depthStencilState.maxDepthBounds	= createInfo.DepthStencilState.MaxDepthBounds;
    }

    VkPipelineColorBlendAttachmentState colorBlendAttachmentStates[8];
    VkPipelineColorBlendStateCreateInfo blendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    {
	blendState.logicOpEnable     = createInfo.BlendState.LogicOpEnable;
	blendState.logicOp	     = VulkanUtilities::Translate(createInfo.BlendState.LogicOp);
	blendState.attachmentCount   = createInfo.BlendState.AttachmentCount;
	blendState.pAttachments	     = colorBlendAttachmentStates;
	blendState.blendConstants[0] = createInfo.BlendState.BlendConstants[0];
	blendState.blendConstants[1] = createInfo.BlendState.BlendConstants[1];
	blendState.blendConstants[2] = createInfo.BlendState.BlendConstants[2];
	blendState.blendConstants[3] = createInfo.BlendState.BlendConstants[3];

	for(size_t i = 0; i < createInfo.BlendState.AttachmentCount; i++)
	{
	    auto& state		      = colorBlendAttachmentStates[i];
	    state.blendEnable	      = createInfo.BlendState.Attachments[i].BlendEnable;
	    state.srcColorBlendFactor = VulkanUtilities::Translate(createInfo.BlendState.Attachments[i].SrcColorBlendFactor);
	    state.dstColorBlendFactor = VulkanUtilities::Translate(createInfo.BlendState.Attachments[i].DstColorBlendFactor);
	    state.colorBlendOp	      = VulkanUtilities::Translate(createInfo.BlendState.Attachments[i].ColorBlendOp);
	    state.srcAlphaBlendFactor = VulkanUtilities::Translate(createInfo.BlendState.Attachments[i].SrcAlphaBlendFactor);
	    state.dstAlphaBlendFactor = VulkanUtilities::Translate(createInfo.BlendState.Attachments[i].DstAlphaBlendFactor);
	    state.alphaBlendOp	      = VulkanUtilities::Translate(createInfo.BlendState.Attachments[i].AlphaBlendOp);
	    state.colorWriteMask      = VulkanUtilities::Translate(createInfo.BlendState.Attachments[i].ColorWriteMask);
	}
    }

    VkDynamicState dynamicStates[9];
    VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicState.pDynamicStates			  = dynamicStates;
    VulkanUtilities::Translate(createInfo.DynamicStateFlags, dynamicState.dynamicStateCount, dynamicStates);

    VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    {
	pipelineInfo.pNext		 = dynamicRenderingEnabled ? &pipelineRenderingCreateInfo : nullptr;
	pipelineInfo.flags		 = 0;
	pipelineInfo.stageCount		 = stageCount;
	pipelineInfo.pStages		 = shaderStages;
	pipelineInfo.pVertexInputState	 = &vertexInputState;
	pipelineInfo.pInputAssemblyState = &inputAssemblyState;
	pipelineInfo.pTessellationState	 = &tessellationState;
	pipelineInfo.pViewportState	 = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizationState;
	pipelineInfo.pMultisampleState	 = &multisamplingState;
	pipelineInfo.pDepthStencilState	 = &depthStencilState;
	pipelineInfo.pColorBlendState	 = &blendState;
	pipelineInfo.pDynamicState	 = &dynamicState;
	pipelineInfo.layout		 = m_pipelineLayout;
	pipelineInfo.renderPass		 = renderPass;
	pipelineInfo.subpass		 = 0;
	pipelineInfo.basePipelineHandle	 = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex	 = 0;
    }

    VulkanUtilities::checkResult(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));

    for(uint32_t i = 0; i < stageCount; i++)
    {
	vkDestroyShaderModule(device, shaderModules[i], nullptr);
    }
}
VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
    VkDevice device = m_adapter->GetDevice();
    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
    vkDestroyDescriptorPool(device, m_staticSamplerDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, m_staticSamplerDescriptorSetLayout, nullptr);

    for(auto* sampler: m_staticSamplers)
    {
	vkDestroySampler(device, sampler, nullptr);
    }
}
void* VulkanGraphicsPipeline::GetNativeHandle() const
{
    return m_pipeline;
}
uint32_t VulkanGraphicsPipeline::GetDescriptorSetLayoutCount() const
{
    return 0;
}
const DescriptorSetLayout* VulkanGraphicsPipeline::GetDescriptorSetLayout(uint32_t index) const
{
    return nullptr;
}
VkPipelineLayout VulkanGraphicsPipeline::GetLayout() const
{
    return m_pipelineLayout;
}
void VulkanGraphicsPipeline::BindStaticSamplerSet(VkCommandBuffer cmdBuffer) const
{
    if(m_staticSamplerDescriptorSet != VK_NULL_HANDLE)
    {
	vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, m_staticSamplerDescriptorSetIndex, 1, &m_staticSamplerDescriptorSet, 0, nullptr);
    }
}

static void CreateShaderStage(VkDevice device, const ShaderStageCreateInfo& stageDesc, VkShaderStageFlagBits stageFlag, VkShaderModule& shaderModule, VkPipelineShaderStageCreateInfo& stageCreateInfo)
{
    char path[ShaderStageCreateInfo::MAX_PATH_LENGTH + 5];
    strcpy_s(path, stageDesc.Path);
    strcat_s(path, ".spv");
    size_t codeSize = Platform::Size(path);
    char* code	    = new char[codeSize];
    Platform::ReadFile(path, codeSize, code, true);
    VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    createInfo.codeSize			= codeSize;
    createInfo.pCode			= reinterpret_cast<const uint32_t*>(code);

    VulkanUtilities::checkResult(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule), "Failed to create shader module!");

    delete[] code;

    stageCreateInfo	   = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stageCreateInfo.stage  = stageFlag;
    stageCreateInfo.module = shaderModule;
    stageCreateInfo.pName  = "main";
}

static void CreatePipelineLayout(
    VkDevice device,
    const PipelineLayoutCreateInfo& layoutCreateInfo,
    VkPipelineLayout& pipelineLayout,
    VkDescriptorSetLayout& staticSamplerDescriptorSetLayout,
    VkDescriptorPool& staticSamplerDescriptorPool,
    VkDescriptorSet& staticSamplerDescriptorSet,
    eastl::fixed_vector<VkSampler, 16>& staticSamplers)
{
    staticSamplerDescriptorSetLayout = VK_NULL_HANDLE;
    staticSamplerDescriptorPool	     = VK_NULL_HANDLE;
    staticSamplerDescriptorSet	     = VK_NULL_HANDLE;
    staticSamplers.clear();

    // create static sampler set
    if(layoutCreateInfo.StaticSamplerCount > 0)
    {
	staticSamplers.reserve(layoutCreateInfo.StaticSamplerCount);
	eastl::fixed_vector<VkDescriptorSetLayoutBinding, 16> staticSamplerBindings;
	staticSamplerBindings.reserve(layoutCreateInfo.StaticSamplerCount);

	for(size_t i = 0; i < layoutCreateInfo.StaticSamplerCount; ++i)
	{
	    const auto& staticSamplerDesc = layoutCreateInfo.StaticSamplerDescriptions[i];

	    VkSamplerCreateInfo samplerCreateInfoVk { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	    samplerCreateInfoVk.magFilter		= VulkanUtilities::Translate(staticSamplerDesc.MagFilter);
	    samplerCreateInfoVk.minFilter		= VulkanUtilities::Translate(staticSamplerDesc.MinFilter);
	    samplerCreateInfoVk.mipmapMode		= VulkanUtilities::Translate(staticSamplerDesc.MipmapMode);
	    samplerCreateInfoVk.addressModeU		= VulkanUtilities::Translate(staticSamplerDesc.AddressModeU);
	    samplerCreateInfoVk.addressModeV		= VulkanUtilities::Translate(staticSamplerDesc.AddressModeV);
	    samplerCreateInfoVk.addressModeW		= VulkanUtilities::Translate(staticSamplerDesc.AddressModeW);
	    samplerCreateInfoVk.mipLodBias		= staticSamplerDesc.MipLodBias;
	    samplerCreateInfoVk.anisotropyEnable	= staticSamplerDesc.AnisotropyEnable;
	    samplerCreateInfoVk.maxAnisotropy		= staticSamplerDesc.MaxAnisotropy;
	    samplerCreateInfoVk.compareEnable		= staticSamplerDesc.CompareEnable;
	    samplerCreateInfoVk.compareOp		= VulkanUtilities::Translate(staticSamplerDesc.CompareOp);
	    samplerCreateInfoVk.minLod			= staticSamplerDesc.MinLod;
	    samplerCreateInfoVk.maxLod			= staticSamplerDesc.MaxLod;
	    samplerCreateInfoVk.borderColor		= VulkanUtilities::Translate(staticSamplerDesc.BorderColor);
	    samplerCreateInfoVk.unnormalizedCoordinates = staticSamplerDesc.UnnormalizedCoordinates;

	    VkSampler sampler = {};
	    VulkanUtilities::checkResult(vkCreateSampler(device, &samplerCreateInfoVk, nullptr, &sampler), "Failed to create Sampler!");
	    staticSamplers.push_back(sampler);

	    VkDescriptorSetLayoutBinding bindingVk {};
	    bindingVk.binding		 = staticSamplerDesc.Binding;
	    bindingVk.descriptorType	 = VK_DESCRIPTOR_TYPE_SAMPLER;
	    bindingVk.descriptorCount	 = 1;
	    bindingVk.stageFlags	 = VulkanUtilities::Translate(staticSamplerDesc.StageFlags);
	    bindingVk.pImmutableSamplers = &staticSamplers.back();

	    staticSamplerBindings.push_back(bindingVk);
	}

	VkDescriptorSetLayoutCreateInfo samplerSetLayoutCreateInfo { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	samplerSetLayoutCreateInfo.bindingCount = layoutCreateInfo.StaticSamplerCount;
	samplerSetLayoutCreateInfo.pBindings	= staticSamplerBindings.data();

	VulkanUtilities::checkResult(vkCreateDescriptorSetLayout(device, &samplerSetLayoutCreateInfo, nullptr, &staticSamplerDescriptorSetLayout), "Failed to create static sampler descriptor set layout!");

	VkDescriptorPoolSize descriptorPoolSize { VK_DESCRIPTOR_TYPE_SAMPLER, layoutCreateInfo.StaticSamplerCount };
	VkDescriptorPoolCreateInfo staticSamplerDescriptorPoolCreateInfo { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	staticSamplerDescriptorPoolCreateInfo.maxSets	    = 1;
	staticSamplerDescriptorPoolCreateInfo.poolSizeCount = 1;
	staticSamplerDescriptorPoolCreateInfo.pPoolSizes    = &descriptorPoolSize;

	VulkanUtilities::checkResult(vkCreateDescriptorPool(device, &staticSamplerDescriptorPoolCreateInfo, nullptr, &staticSamplerDescriptorPool), "Failed to create static sampler descriptor pool!");

	VkDescriptorSetAllocateInfo staticSamplerDescriptorSetAllocateInfo { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	staticSamplerDescriptorSetAllocateInfo.descriptorPool	  = staticSamplerDescriptorPool;
	staticSamplerDescriptorSetAllocateInfo.descriptorSetCount = 1;
	staticSamplerDescriptorSetAllocateInfo.pSetLayouts	  = &staticSamplerDescriptorSetLayout;

	VulkanUtilities::checkResult(vkAllocateDescriptorSets(device, &staticSamplerDescriptorSetAllocateInfo, &staticSamplerDescriptorSet), "Failed to allocate static sampler descriptor set!");
    }

    VkDescriptorSetLayout layoutsVk[5];
    for(size_t i = 0; i < layoutCreateInfo.DescriptorSetLayoutCount; ++i)
    {
	auto* layoutVk = dynamic_cast<VulkanDescriptorSetLayout*>(layoutCreateInfo.DescriptorSetLayoutDeclarations[i].Layout);
	assert(layoutVk);
	layoutsVk[i] = static_cast<VkDescriptorSetLayout>(layoutVk->GetNativeHandle());
    }
    if(staticSamplerDescriptorSetLayout != VK_NULL_HANDLE)
    {
	layoutsVk[layoutCreateInfo.StaticSamplerSet] = staticSamplerDescriptorSetLayout;
    }

    VkPushConstantRange pushConstantRange;
    pushConstantRange.stageFlags = VulkanUtilities::Translate(layoutCreateInfo.PushConstStageFlags);
    pushConstantRange.offset	 = 0;
    pushConstantRange.size	 = layoutCreateInfo.PushConstRange;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipelineLayoutCreateInfo.setLayoutCount	    = (staticSamplerDescriptorSetLayout != VK_NULL_HANDLE) ? (layoutCreateInfo.DescriptorSetLayoutCount + 1) : layoutCreateInfo.DescriptorSetLayoutCount;
    pipelineLayoutCreateInfo.pSetLayouts	    = layoutsVk;
    pipelineLayoutCreateInfo.pushConstantRangeCount = layoutCreateInfo.PushConstRange > 0 ? 1 : 0;
    pipelineLayoutCreateInfo.pPushConstantRanges    = layoutCreateInfo.PushConstRange > 0 ? &pushConstantRange : nullptr;

    VulkanUtilities::checkResult(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout), "Failed to create PipelineLayout!");
}