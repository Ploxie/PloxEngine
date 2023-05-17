//
// Created by Ploxie on 2023-05-17.
//

#pragma once
#include "eastl/fixed_vector.h"
#include "rendering/types/graphicsPipeline.h"
#include "vulkan/vulkan.h"

class VulkanGraphicsAdapter;

class VulkanGraphicsPipeline : public GraphicsPipeline
{
public:
    explicit VulkanGraphicsPipeline(VulkanGraphicsAdapter* adapter, const GraphicsPipelineCreateInfo& createInfo);
    VulkanGraphicsPipeline(VulkanGraphicsPipeline&)		      = delete;
    VulkanGraphicsPipeline(VulkanGraphicsPipeline&&)		      = delete;
    VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&)  = delete;
    VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&&) = delete;
    ~VulkanGraphicsPipeline() override;

    void* GetNativeHandle() const override;
    uint32_t GetDescriptorSetLayoutCount() const override;
    const DescriptorSetLayout* GetDescriptorSetLayout(uint32_t index) const override;
    VkPipelineLayout GetLayout() const;
    void BindStaticSamplerSet(VkCommandBuffer cmdBuffer) const;

private:
    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;
    VulkanGraphicsAdapter* m_adapter;
    uint32_t m_staticSamplerDescriptorSetIndex;
    VkDescriptorSetLayout m_staticSamplerDescriptorSetLayout;
    VkDescriptorPool m_staticSamplerDescriptorPool;
    VkDescriptorSet m_staticSamplerDescriptorSet;
    eastl::fixed_vector<VkSampler, 16> m_staticSamplers;
};