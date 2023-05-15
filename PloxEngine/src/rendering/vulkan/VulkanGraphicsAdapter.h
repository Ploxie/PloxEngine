//
// Created by Ploxie on 2023-05-09.
//

#pragma once
#include "eastl/vector.h"
#include "rendering/GraphicsAdapter.h"
#include "VulkanInstanceProperties.h"
#include "VulkanQueue.h"
#include "VulkanSemaphore.h"

class VulkanSwapchain;
class Window;

#undef CreateSemaphore

namespace Vulkan
{
    void AddPlatformInstanceExtensions(VulkanInstanceProperties& instanceProperties);
    VkResult CreateWindowSurface(void* windowHandle, VkInstance instance, VkSurfaceKHR* surface);
    void InitializePlatform(VkInstance instance, VkDevice device);
    void AddSwapchainWindowInfo(VkSwapchainCreateInfoKHR& swapchainInfo, Window* window);
    bool ActivateFullscreen(Window* window, VulkanSwapchain* swapchain);
}; // namespace Vulkan

class VulkanGraphicsAdapter : public GraphicsAdapter
{
public:
    explicit VulkanGraphicsAdapter(void* windowHandle, bool debugLayer);
    ~VulkanGraphicsAdapter() override;

    void CreateSwapchain(const Queue* presentQueue, unsigned int width, unsigned int height, Window* window, PresentMode presentMode, Swapchain** swapchain) override;
    void CreateSemaphore(uint64_t initialValue, Semaphore** semaphore) override;

    bool ActivateFullscreen(Window* window) override;

    Queue* GetGraphicsQueue() override;
    Queue* GetComputeQueue() override;
    Queue* GetTransferQueue() override;

private:
    VkInstance m_instance			   = VK_NULL_HANDLE;
    VkDevice m_device				   = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice		   = VK_NULL_HANDLE;
    VulkanQueue m_graphicsQueue			   = {};
    VulkanQueue m_computeQueue			   = {};
    VulkanQueue m_transferQueue			   = {};
    VkSurfaceKHR m_surface			   = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugUtilsMessenger = VK_NULL_HANDLE;
    VulkanSwapchain* m_swapchain		   = nullptr;
    //DynamicPoolAllocator m_commandListPoolMemoryPool;
    bool m_dynamicRenderingExtensionSupport = false;
    bool m_supportsMemoryBudgetExtension    = false;
    bool m_fullscreenExclusiveSupported	    = false;
};