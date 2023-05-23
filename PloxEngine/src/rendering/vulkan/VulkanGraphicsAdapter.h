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
    void SetResourceName(VkDevice device, VkObjectType type, uint64_t handle, const char* name);
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
    void CreateImageView(const ImageViewCreateInfo* imageViewCreateInfo, ImageView** imageView) override;
    void CreateImageView(Image* image, ImageView** imageView) override;
    void CreateDescriptorSetPool(uint32_t maxSets, const DescriptorSetLayout* descriptorSetLayout, DescriptorSetPool** descriptorSetPool) override;
    void CreateDescriptorSetLayout(uint32_t bindingCount, const DescriptorSetLayoutBinding* bindings, DescriptorSetLayout** descriptorSetLayout) override;

    bool ActivateFullscreen(Window* window) override;

    VkDevice& GetDevice();
    const VkPhysicalDeviceProperties& GetDeviceProperties() const;
    Queue* GetGraphicsQueue() override;
    Queue* GetComputeQueue() override;
    Queue* GetTransferQueue() override;

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
    DynamicPoolAllocator m_commandPoolMemoryPool;
    DynamicPoolAllocator m_semaphoreMemoryPool;
    DynamicPoolAllocator m_graphicsPipelineMemoryPool;
    DynamicPoolAllocator m_descriptorSetPoolMemoryPool;
    DynamicPoolAllocator m_descriptorSetLayoutMemoryPool;
    DynamicPoolAllocator m_imageViewMemoryPool;
    bool m_dynamicRenderingExtensionSupport = false;
    bool m_supportsMemoryBudgetExtension    = false;
    bool m_fullscreenExclusiveSupported	    = false;
};