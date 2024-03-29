//
// Created by Ploxie on 2023-05-21.
//

#pragma once
#include "rendering/types/Buffer.h"
#include "rendering/types/BufferView.h"
#include "vulkan/vulkan.h"

class VulkanMemoryAllocator;
class VulkanGraphicsAdapter;

class VulkanBuffer : public Buffer
{
public:
    explicit VulkanBuffer(VkBuffer buffer, void* allocHandle, const BufferCreateInfo& createInfo, VulkanMemoryAllocator* allocator, VulkanGraphicsAdapter* adapter);

    void* GetNativeHandle() const override;
    const BufferCreateInfo& GetDescription() const override;
    void Map(void** data) override;
    void Unmap() override;
    void Invalidate(uint32_t count, const MemoryRange* ranges) override;
    void Flush(uint32_t count, const MemoryRange* ranges) override;
    void* GetAllocationHandle();
    VkDeviceMemory GetMemory() const;
    VkDeviceSize GetOffset() const;

private:
    VkBuffer m_buffer;
    void* m_allocHandle;
    BufferCreateInfo m_description;
    VulkanMemoryAllocator* m_allocator;
    VulkanGraphicsAdapter* m_adapter;
};

class VulkanBufferView : public BufferView
{
public:
    explicit VulkanBufferView(VkDevice device, const BufferViewCreateInfo& createInfo);
    ~VulkanBufferView();

    VulkanBufferView(VulkanBufferView&)			  = delete;
    VulkanBufferView(VulkanBufferView&&)		  = delete;
    VulkanBufferView& operator=(const VulkanBufferView&)  = delete;
    VulkanBufferView& operator=(const VulkanBufferView&&) = delete;

    void* GetNativeHandle() const override;
    const Buffer* GetBuffer() const override;
    const BufferViewCreateInfo& GetDescription() const override;

private:
    VkDevice m_device;
    VkBufferView m_bufferView;
    BufferViewCreateInfo m_description;
};
