//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "rendering/types/Swapchain.h"
#include "utility/memory/MemoryPool.h"
#include "vulkan/vulkan.h"
#include "VulkanImage.h"
#include "VulkanQueue.h"

class Window;

class VulkanSwapchain : public Swapchain
{
public:
    explicit VulkanSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, Queue* presentQueue, uint32_t width, uint32_t height, Window* window, PresentMode presentMode);
    ~VulkanSwapchain() override;

    void* GetNativeHandle() const override;
    void Resize(uint32_t width, uint32_t height, Window* window, PresentMode presentMode) override;
    uint32_t GetCurrentImageIndex() override;
    void Present(Semaphore* waitSemaphore, uint64_t semaphoreWaitValue, Semaphore* signalSemaphore, uint64_t semaphoreSignalValue) override;
    Extent2D GetExtent() const override;
    Extent2D GetRecreationExtent() const override;
    Format GetImageFormat() const override;
    Image* GetImage(uint32_t index) const override;
    Queue* GetPresentQueue() const override;
    PresentMode GetPresentMode() const override;

private:
    static constexpr uint32_t MAX_IMAGE_COUNT = 8;
    static constexpr uint32_t SEMAPHORE_COUNT = 3;

    VkPhysicalDevice m_physicalDevice	   = VK_NULL_HANDLE;
    VkDevice m_device			   = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface		   = VK_NULL_HANDLE;
    Queue* m_presentQueue		   = nullptr;
    VkSwapchainKHR m_swapchain		   = VK_NULL_HANDLE;
    uint32_t m_imageCount		   = 0;
    VulkanImage* m_images[MAX_IMAGE_COUNT] = {};
    StaticMemoryPool<VulkanImage, MAX_IMAGE_COUNT> m_imageMemoryPool;
    Format m_imageFormat			     = Format::UNDEFINED;
    Extent2D m_extent				     = {};
    uint32_t m_currentImageIndex		     = 0;
    VkSemaphore m_acquireSemaphores[SEMAPHORE_COUNT] = {};
    VkSemaphore m_presentSemaphores[SEMAPHORE_COUNT] = {};
    uint64_t m_frameIndex			     = 0;
    Window* m_window				     = nullptr;
    PresentMode m_presentMode			     = PresentMode::IMMEDIATE;

    void Create(uint32_t width, uint32_t height);
    void Destroy();
    void Resize(uint32_t width, uint32_t height, bool acquireImage);
    uint32_t AcquireImageIndex(VkSemaphore semaphore);
};
