//
// Created by Ploxie on 2023-05-11.
//
#include "VulkanSwapchain.h"
#include "core/Assert.h"
#include "core/Logger.h"
#include "eastl/vector.h"
#include "VulkanUtilities.h"

VulkanSwapchain::VulkanSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,
				 Queue* presentQueue, uint32_t width, uint32_t height, bool fullscreen,
				 PresentMode presentMode)
    : m_physicalDevice(physicalDevice),
      m_device(device),
      m_surface(surface),
      m_presentQueue(presentQueue),
      m_fullscreen(fullscreen),
      m_presentMode(presentMode)
{
    Create(width, height);
}

VulkanSwapchain::~VulkanSwapchain()
{
    Destroy();
}

void* VulkanSwapchain::GetNativeHandle() const
{
    return m_swapchain;
}

void VulkanSwapchain::Resize(uint32_t width, uint32_t height, bool fullscreen, PresentMode presentMode)
{
    m_fullscreen  = fullscreen;
    m_presentMode = presentMode;
    Resize(width, height, true);
}

uint32_t VulkanSwapchain::GetCurrentImageIndex()
{
    if(m_frameIndex == 0 && m_currentImageIndex == -1)
    {
	m_currentImageIndex = AcquireImageIndex(VK_NULL_HANDLE);
    }
    return m_currentImageIndex;
}

void VulkanSwapchain::Present(Semaphore* waitSemaphore, uint64_t semaphoreWaitValue, Semaphore* signalSemaphore,
			      uint64_t semaphoreSignalValue)
{
    const uint32_t resIdx = m_frameIndex % SEMAPHORE_COUNT;
    auto* presentQueueVk  = static_cast<VkQueue>(m_presentQueue->GetNativeHandle());

    // Submit queue
    {
	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	auto* waitSemaphoreVk		      = static_cast<VkSemaphore>(waitSemaphore->GetNativeHandle());

	uint64_t dummyValue = 0;

	VkTimelineSemaphoreSubmitInfo timelineSubmitInfo = { VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
	{
	    timelineSubmitInfo.waitSemaphoreValueCount	 = 1;
	    timelineSubmitInfo.pWaitSemaphoreValues	 = &semaphoreWaitValue;
	    timelineSubmitInfo.signalSemaphoreValueCount = 1;
	    timelineSubmitInfo.pSignalSemaphoreValues	 = &dummyValue;
	}

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO, &timelineSubmitInfo };
	{
	    submitInfo.waitSemaphoreCount   = 1;
	    submitInfo.pWaitSemaphores	    = &waitSemaphoreVk;
	    submitInfo.pWaitDstStageMask    = &waitDstStageMask;
	    submitInfo.commandBufferCount   = 0;
	    submitInfo.pCommandBuffers	    = nullptr;
	    submitInfo.signalSemaphoreCount = 1;
	    submitInfo.pSignalSemaphores    = &m_presentSemaphores[resIdx];
	}

	VulkanUtilities::checkResult(vkQueueSubmit(presentQueueVk, 1, &submitInfo, VK_NULL_HANDLE));
    }

    // Present
    {
	VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	{
	    presentInfo.waitSemaphoreCount = 1;
	    presentInfo.pWaitSemaphores	   = &m_presentSemaphores[resIdx];
	    presentInfo.swapchainCount	   = 1;
	    presentInfo.pSwapchains	   = &m_swapchain;
	    presentInfo.pImageIndices	   = &m_currentImageIndex;
	}

	VulkanUtilities::checkResult(vkQueuePresentKHR(presentQueueVk, &presentInfo));
    }

    m_currentImageIndex = AcquireImageIndex(m_acquireSemaphores[resIdx]);

    // Submit queue
    {
	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	auto* signalSemaphoreVk		      = static_cast<VkSemaphore>(signalSemaphore->GetNativeHandle());

	uint64_t dummyValue = 0;

	VkTimelineSemaphoreSubmitInfo timelineSubmitInfo = { VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
	{
	    timelineSubmitInfo.waitSemaphoreValueCount	 = 1;
	    timelineSubmitInfo.pWaitSemaphoreValues	 = &dummyValue;
	    timelineSubmitInfo.signalSemaphoreValueCount = 1;
	    timelineSubmitInfo.pSignalSemaphoreValues	 = &semaphoreSignalValue;
	}

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO, &timelineSubmitInfo };
	{
	    submitInfo.waitSemaphoreCount   = 1;
	    submitInfo.pWaitSemaphores	    = &m_acquireSemaphores[resIdx];
	    submitInfo.pWaitDstStageMask    = &waitDstStageMask;
	    submitInfo.commandBufferCount   = 0;
	    submitInfo.pCommandBuffers	    = nullptr;
	    submitInfo.signalSemaphoreCount = 1;
	    submitInfo.pSignalSemaphores    = &signalSemaphoreVk;
	}

	VulkanUtilities::checkResult(vkQueueSubmit(presentQueueVk, 1, &submitInfo, VK_NULL_HANDLE));
    }

    m_frameIndex++;
}

Extent2D VulkanSwapchain::GetExtent() const
{
    return m_extent;
}

Extent2D VulkanSwapchain::GetRecreationExtent() const
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &surfaceCapabilities);

    const auto extentVk = surfaceCapabilities.currentExtent.width != UINT32_MAX ?
			      surfaceCapabilities.currentExtent :
			      surfaceCapabilities.minImageExtent;
    return { extentVk.width, extentVk.height };
}

Format VulkanSwapchain::GetImageFormat() const
{
    return m_imageFormat;
}

Image* VulkanSwapchain::GetImage(uint32_t index) const
{
    ASSERT(index < m_imageCount);
    return m_images[index];
}

Queue* VulkanSwapchain::GetPresentQueue() const
{
    return m_presentQueue;
}

void VulkanSwapchain::Create(uint32_t width, uint32_t height)
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &surfaceCapabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, nullptr);
    eastl::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, formats.data());

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, nullptr);
    eastl::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, presentModes.data());

    // Find surface format
    VkSurfaceFormatKHR surfaceFormat;
    {
	bool foundOptimal = false;
	for(auto& format: formats)
	{
	    if(format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
	    {
		surfaceFormat = format;
		foundOptimal  = true;
		break;
	    }
	}
	if(!foundOptimal)
	{
	    surfaceFormat = formats[0];
	}
    }

    // Find present mode
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    {
	for(auto& mode: presentModes)
	{
	    if((m_presentMode == PresentMode::V_SYNC && mode == VK_PRESENT_MODE_MAILBOX_KHR) ||
	       (m_presentMode == PresentMode::IMMEDIATE && mode == VK_PRESENT_MODE_IMMEDIATE_KHR))
	    {
		presentMode = mode;
		break;
	    }
	}
    }

    // Find correct extent
    VkExtent2D extent;
    {
	if(surfaceCapabilities.currentExtent.width != UINT32_MAX)
	{
	    extent = surfaceCapabilities.currentExtent;
	}
	else
	{
	    extent = { CLAMP(width, surfaceCapabilities.minImageExtent.width,
			     surfaceCapabilities.maxImageExtent.width),
		       CLAMP(height, surfaceCapabilities.minImageExtent.height,
			     surfaceCapabilities.maxImageExtent.height) };
	}
    }

    m_imageCount = surfaceCapabilities.minImageCount + 1;
    if(surfaceCapabilities.maxImageCount > 0 && m_imageCount > surfaceCapabilities.maxImageCount)
    {
	m_imageCount = surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    {
	createInfo.surface	    = m_surface;
	createInfo.minImageCount    = m_imageCount;
	createInfo.imageFormat	    = surfaceFormat.format;
	createInfo.imageColorSpace  = surfaceFormat.colorSpace;
	createInfo.imageExtent	    = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage	    = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform	    = surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode	    = presentMode;
	createInfo.clipped	    = VK_TRUE;
    }

    // Create swapchain
    {
	if(vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
	{
	    LOG_CORE_CRITICAL("Failed to create swapchain");
	    return;
	}

	vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_imageCount, nullptr);
	if(m_imageCount > MAX_IMAGE_COUNT)
	{
	    LOG_CORE_CRITICAL("Swapchain image count higher than supported maximum");
	    return;
	}
    }

    // Create images
    VkImage imagesVk[MAX_IMAGE_COUNT];
    {
	vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_imageCount, imagesVk);

	ImageCreateInfo imageCreateInfo = {};
	{
	    imageCreateInfo.m_width	  = extent.width;
	    imageCreateInfo.m_height	  = extent.height;
	    imageCreateInfo.m_depth	  = 1;
	    imageCreateInfo.m_layers	  = 1;
	    imageCreateInfo.m_levels	  = 1;
	    imageCreateInfo.m_samples	  = SampleCount::_1;
	    imageCreateInfo.m_imageType	  = ImageType::_2D;
	    imageCreateInfo.m_format	  = static_cast<Format>(surfaceFormat.format);
	    imageCreateInfo.m_createFlags = {};
	    imageCreateInfo.m_usageFlags  = ImageUsageFlags::TRANSFER_DST_BIT | ImageUsageFlags::RW_TEXTURE_BIT |
					   ImageUsageFlags::COLOR_ATTACHMENT_BIT | ImageUsageFlags::TEXTURE_BIT;
	}

	for(uint32_t i = 0; i < m_imageCount; i++)
	{
	    auto* memory = m_imageMemoryPool.Alloc();
	    ASSERT(memory);

	    m_images[i] = new(memory) VulkanImage(imagesVk[i], nullptr, imageCreateInfo);
	}
    }

    m_imageFormat = static_cast<Format>(surfaceFormat.format);
    m_extent	  = { extent.width, extent.height };

    // Create Semaphores
    {
	for(int i = 0; i < SEMAPHORE_COUNT; ++i)
	{
	    VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	    {
		VulkanUtilities::checkResult(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_acquireSemaphores[i]));
		VulkanUtilities::checkResult(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_presentSemaphores[i]));
	    }
	}
    }
}

void VulkanSwapchain::Destroy()
{
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    for(uint32_t i = 0; i < SEMAPHORE_COUNT; ++i)
    {
	vkDestroySemaphore(m_device, m_acquireSemaphores[i], nullptr);
	vkDestroySemaphore(m_device, m_presentSemaphores[i], nullptr);
    }

    for(uint32_t i = 0; i < m_imageCount; i++)
    {
	m_images[i]->~VulkanImage();
    }
}

void VulkanSwapchain::Resize(uint32_t width, uint32_t height, bool acquireImage)
{
    vkDeviceWaitIdle(m_device);
    Destroy();
    Create(width, height);
    if(acquireImage)
    {
	m_currentImageIndex = AcquireImageIndex(VK_NULL_HANDLE);
    }
}

uint32_t VulkanSwapchain::AcquireImageIndex(VkSemaphore semaphore)
{
    uint32_t imageIndex;

    VkFence fence = VK_NULL_HANDLE;
    if(semaphore == VK_NULL_HANDLE)
    {
	VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	vkCreateFence(m_device, &createInfo, nullptr, &fence);
    }

    bool tryAgain	  = false;
    int remainingAttempts = 3;
    do
    {
	remainingAttempts--;
	VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, semaphore, fence, &imageIndex);

	switch(result)
	{
	    case VK_SUCCESS:
	    case VK_SUBOPTIMAL_KHR:
		if(semaphore == VK_NULL_HANDLE)
		{
		    VulkanUtilities::checkResult(vkWaitForFences(m_device, 1, &fence, VK_TRUE, UINT64_MAX));
		}
		break;
	    case VK_ERROR_OUT_OF_DATE_KHR:
		Resize(m_extent.m_width, m_extent.m_height, false);
		tryAgain = true;
		break;
	    case VK_ERROR_SURFACE_LOST_KHR:
		LOG_CORE_CRITICAL("Failed to acquire Swapchain image: VK_ERROR_SURFACE_LOST_KHR");
		break;

	    default:
		break;
	}
    } while(tryAgain && remainingAttempts > 0);

    if(semaphore == VK_NULL_HANDLE)
    {
	vkDestroyFence(m_device, fence, nullptr);
    }
    if(remainingAttempts <= 0)
    {
	LOG_CORE_CRITICAL("Failed to acquire Swapchain image: Too many failed attempts at swapchain recreation");
    }

    return imageIndex;
}