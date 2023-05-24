//
// Created by Ploxie on 2023-05-17.
//

#pragma once
#include "rendering/types/DescriptorSet.h"
#include "vulkan/vulkan.h"

class VulkanDescriptorSetLayout : public DescriptorSetLayout
{
public:
    explicit VulkanDescriptorSetLayout(VkDevice device, uint32_t bindingCount, const VkDescriptorSetLayoutBinding* bindings, const VkDescriptorBindingFlags* bindingFlags);
    ~VulkanDescriptorSetLayout() override;

    VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&)		    = delete;
    VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&&)		    = delete;
    VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&)  = delete;
    VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&&) = delete;

    void* GetNativeHandle() const override;
    const uint32_t* GetTypeCounts() const;

private:
    VkDevice m_device;
    VkDescriptorSetLayout m_descriptorSetLayout;
    uint32_t m_typeCounts[(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT - VK_DESCRIPTOR_TYPE_SAMPLER + 1)];
};

class VulkanDescriptorSet : public DescriptorSet
{
public:
    explicit VulkanDescriptorSet(VkDevice device, VkDescriptorSet descriptorSet, uint32_t dynamicBufferCount);
    void* GetNativeHandle() const override;
    void Update(uint32_t count, const DescriptorSetUpdate* updates) override;
    uint32_t GetDynamicBufferCount() const;

private:
    VkDevice m_device;
    VkDescriptorSet m_descriptorSet;
    uint32_t m_dynamicBufferCount;
};

class VulkanDescriptorSetPool : public DescriptorSetPool
{
public:
    explicit VulkanDescriptorSetPool(VkDevice device, uint32_t maxSets, const VulkanDescriptorSetLayout* layout);
    ~VulkanDescriptorSetPool() override;

    VulkanDescriptorSetPool(VulkanDescriptorSetPool&)			= delete;
    VulkanDescriptorSetPool(VulkanDescriptorSetPool&&)			= delete;
    VulkanDescriptorSetPool& operator=(const VulkanDescriptorSetPool&)	= delete;
    VulkanDescriptorSetPool& operator=(const VulkanDescriptorSetPool&&) = delete;
    
    void* GetNativeHandle() const override;
    void AllocateDescriptorSets(uint32_t count, DescriptorSet** sets) override;
    void Reset() override;

private:
    VkDevice m_device;
    VkDescriptorPool m_descriptorPool;
    const VulkanDescriptorSetLayout* m_layout;
    uint32_t m_poolSize;
    uint32_t m_currentOffset;
    char* m_descriptorSetMemory;
};
