//
// Created by Ploxie on 2023-05-09.
//

#pragma once
#include "eastl/vector.h"
#include "rendering/GraphicsAdapter.h"
#include "rendering/types/DescriptorSet.h"
#include "utility/memory/PoolAllocator.h"
#include "VulkanFrameBufferDescription.h"
#include "VulkanInstanceProperties.h"
#include "VulkanQueue.h"
#include "VulkanRenderPassDescription.h"
#include "VulkanSemaphore.h"

class VulkanSwapchain;
class Window;
class VulkanRenderPassCache;
class VulkanMemoryAllocator;

#undef CreateSemaphore

namespace Vulkan
{
    void AddPlatformInstanceExtensions(VulkanInstanceProperties& instanceProperties);
    VkResult CreateWindowSurface(void* windowHandle, VkInstance instance, VkSurfaceKHR* surface);
    void InitializePlatform(VkInstance instance, VkDevice device);
    void AddFullscreenExclusiveInfo(const void* pNext, Window* window);
    bool ActivateFullscreen(Window* window, VulkanSwapchain* swapchain);
}; // namespace Vulkan

class VulkanGraphicsAdapter : public GraphicsAdapter
{
public:
    explicit VulkanGraphicsAdapter(void* windowHandle, bool debugLayer);
    ~VulkanGraphicsAdapter() override;

    void CreateGraphicsPipeline(uint32_t count, const GraphicsPipelineCreateInfo* createInfo, GraphicsPipeline** pipelines) override;
    void CreateCommandPool(const Queue* queue, CommandPool** commandPool) override;
    void CreateSwapchain(const Queue* presentQueue, unsigned int width, unsigned int height, Window* window, PresentMode presentMode, Swapchain** swapchain) override;
    void CreateSemaphore(uint64_t initialValue, Semaphore** semaphore) override;
    void CreateImage(const ImageCreateInfo& imageCreateInfo, MemoryPropertyFlags requiredMemoryPropertyFlags, MemoryPropertyFlags preferredMemoryPropertyFlags, bool dedicated, Image** image) override;
    void CreateImageView(const ImageViewCreateInfo* imageViewCreateInfo, ImageView** imageView) override;
    void CreateImageView(Image* image, ImageView** imageView) override;
    void CreateBuffer(const BufferCreateInfo& bufferCreateInfo, MemoryPropertyFlags requiredMemoryPropertyFlags, MemoryPropertyFlags preferredMemoryPropertyFlags, bool dedicated, Buffer** buffer) override;
    void CreateBufferView(const BufferViewCreateInfo* bufferViewCreateInfo, BufferView** bufferView) override;
    void CreateDescriptorSetPool(uint32_t maxSets, const DescriptorSetLayout* descriptorSetLayout, DescriptorSetPool** descriptorSetPool) override;
    void CreateDescriptorSetLayout(uint32_t bindingCount, const DescriptorSetLayoutBinding* bindings, DescriptorSetLayout** descriptorSetLayout) override;

    void DestroyCommandPool(CommandPool* commandPool) override;
    void DestroyImage(Image* image) override;
    void DestroyImageView(ImageView* imageView) override;
    void DestroyBuffer(Buffer* buffer) override;
    void DestroyBufferView(BufferView* bufferView) override;
    void DestroyDescriptorSetPool(DescriptorSetPool* descriptorSetPool) override;
    void DestroyDescriptorSetLayout(DescriptorSetLayout* descriptorSetLayout) override;

    bool ActivateFullscreen(Window* window) override;

    VkDevice& GetDevice();
    const VkPhysicalDeviceProperties& GetDeviceProperties() const;
    Queue* GetGraphicsQueue() override;
    Queue* GetComputeQueue() override;
    Queue* GetTransferQueue() override;

    void SetDebugObjectName(ObjectType type, void* object, const char* name) override;

    VkRenderPass GetRenderPass(const VulkanRenderPassDescription& renderPassDescription);
    VkFramebuffer GetFrameBuffer(const VulkanFrameBufferDescription& frameBufferDescription);

    bool IsDynamicRenderingExtensionSupported();

private:
    VkInstance m_instance			   = VK_NULL_HANDLE;
    VkDevice m_device				   = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice		   = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties m_properties	   = {};
    VulkanQueue m_graphicsQueue			   = {};
    VulkanQueue m_computeQueue			   = {};
    VulkanQueue m_transferQueue			   = {};
    VkSurfaceKHR m_surface			   = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugUtilsMessenger = VK_NULL_HANDLE;
    VulkanRenderPassCache* m_renderPassCache	   = nullptr;
    VulkanFrameBufferCache* m_frameBufferCache	   = nullptr;
    VulkanSwapchain* m_swapchain		   = nullptr;
    VulkanMemoryAllocator* m_allocator		   = nullptr;
    DynamicPoolAllocator m_graphicsPipelineMemoryPool;
    //DynamicPoolAllocator m_computePipelineMemoryPool;
    DynamicPoolAllocator m_commandPoolMemoryPool;
    DynamicPoolAllocator m_imageMemoryPool;
    DynamicPoolAllocator m_bufferMemoryPool;
    DynamicPoolAllocator m_imageViewMemoryPool;
    DynamicPoolAllocator m_bufferViewMemoryPool;
    //DynamicPoolAllocator m_samplerMemoryPool;
    DynamicPoolAllocator m_semaphoreMemoryPool;
    //DynamicPoolAllocator m_queryPoolMemoryPool;
    DynamicPoolAllocator m_descriptorSetPoolMemoryPool;
    DynamicPoolAllocator m_descriptorSetLayoutMemoryPool;
    bool m_dynamicRenderingExtensionSupport = false;
    bool m_supportsMemoryBudgetExtension    = false;
    bool m_fullscreenExclusiveSupported	    = false;
};