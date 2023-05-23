//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "rendering/types/Swapchain.h"
#include "utility/ObjectPool.h"
#include "vulkan/vulkan.h"
#include "VulkanImage.h"
#include "VulkanQueue.h"

class Window;
class Command;

class VulkanSwapchain : public Swapchain
{
public:
    explicit VulkanSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, Queue* presentQueue, uint32_t width, uint32_t height, Window* window, PresentMode presentMode);
    ~VulkanSwapchain() override;

    void Resize(uint32_t width, uint32_t height, Window* window, PresentMode presentMode) override;
    void Present(Semaphore* waitSemaphore, uint64_t semaphoreWaitValue, Semaphore* signalSemaphore, uint64_t semaphoreSignalValue) override;

    uint32_t GetCurrentImageIndex() override;
    void* GetNativeHandle() const override;
    Extent2D GetExtent() const override;
    Extent2D GetRecreationExtent() const override;
    Format GetImageFormat() const override;
    Image* GetImage(uint32_t index) const override;
    Queue* GetPresentQueue() const override;
    PresentMode GetPresentMode() const override;

private:
    static constexpr uint32_t MAX_IMAGE_COUNT  = 8;
    static constexpr uint32_t BACKBUFFER_COUNT = 3;

    VkPhysicalDevice m_physicalDevice			= VK_NULL_HANDLE;
    VkDevice m_device					= VK_NULL_HANDLE;
    VkSurfaceKHR m_surface				= VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain				= VK_NULL_HANDLE;
    VkSemaphore m_acquireSemaphores[BACKBUFFER_COUNT]	= {};
    VkSemaphore m_presentSemaphores[BACKBUFFER_COUNT]	= {};
    VkFence m_cmdBufferExecutedFences[BACKBUFFER_COUNT] = {};

    Format m_imageFormat      = Format::UNDEFINED;
    Extent2D m_extent	      = {};
    PresentMode m_presentMode = PresentMode::IMMEDIATE;
    uint32_t m_imageCount     = 0;
    Window* m_window	      = nullptr;
    Queue* m_presentQueue     = nullptr;

    VulkanImage* m_images[MAX_IMAGE_COUNT] = {};
    uint32_t m_currentImageIndex	   = 0;
    uint32_t m_currentSemaphoreIndex	   = 0;
    uint32_t m_previousSemaphoreIndex	   = 0;
    uint64_t m_frameIndex		   = 0;
    StaticMemoryPool<VulkanImage, MAX_IMAGE_COUNT> m_imageMemoryPool;

    void Create(uint32_t width, uint32_t height);
    void Destroy();
    void Resize(uint32_t width, uint32_t height, bool acquireImage);
    uint32_t AcquireNextImageIndex();
};
