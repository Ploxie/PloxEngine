//
// Created by Ploxie on 2023-05-11.
//
#include "VulkanSwapchain.h"
#include "core/Assert.h"
#include "core/Logger.h"
#include "eastl/vector.h"
#include "platform/window/Window.h"
#include "volk.h"
#include "VulkanGraphicsAdapter.h"
#include "VulkanUtilities.h"

VulkanSwapchain::VulkanSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,
				 Queue* presentQueue, uint32_t width, uint32_t height, Window* window,
				 PresentMode presentMode)
    : m_physicalDevice(physicalDevice),
      m_device(device),
      m_surface(surface),
      m_presentQueue(presentQueue),
      m_window(window),
      m_presentMode(presentMode)
{
    Create(width, height);
}

VulkanSwapchain::~VulkanSwapchain()
{
    Destroy();
}

void VulkanSwapchain::Create(uint32_t width, uint32_t height)
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VulkanUtilities::checkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &surfaceCapabilities));

    // Get available Surface Formats
    eastl::vector<VkSurfaceFormat2KHR> surfaceFormats;
    {
	VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR };
	{
	    surfaceInfo.surface = m_surface;
	    Vulkan::AddFullscreenExclusiveInfo(surfaceInfo.pNext, m_window);
	}
	uint32_t formatCount;
	VulkanUtilities::checkResult(vkGetPhysicalDeviceSurfaceFormats2KHR(m_physicalDevice, &surfaceInfo, &formatCount, nullptr));
	surfaceFormats.resize(formatCount);
	for(auto& format: surfaceFormats)
	{
	    format.sType = { VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR };
	}
	VulkanUtilities::checkResult(vkGetPhysicalDeviceSurfaceFormats2KHR(m_physicalDevice, &surfaceInfo, &formatCount, surfaceFormats.data()));
    }

    uint32_t presentModeCount;
    VulkanUtilities::checkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, nullptr));
    eastl::vector<VkPresentModeKHR> presentModes(presentModeCount);
    VulkanUtilities::checkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, presentModes.data()));

    // Find surface format
    VkSurfaceFormatKHR surfaceFormat;
    {
	surfaceFormat = surfaceFormats[0].surfaceFormat; // Default format

	for(auto& format: surfaceFormats)
	{
	    if(format.surfaceFormat.format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 &&
	       format.surfaceFormat.colorSpace == VK_COLOR_SPACE_DISPLAY_NATIVE_AMD)
	    {
		surfaceFormat = format.surfaceFormat;
		break;
	    }

	    if(format.surfaceFormat.colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
	    {
		continue;
	    }

	    if(format.surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM || format.surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB)
	    {
		surfaceFormat = format.surfaceFormat;
		break;
	    }
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
    VkExtent2D extent = { width, height };
    {
	bool useCurrentExtent = surfaceCapabilities.currentExtent.width != UINT32_MAX;
	extent.width	      = CLAMP(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
	extent.height	      = CLAMP(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
	extent		      = useCurrentExtent ? surfaceCapabilities.currentExtent : extent;
    }

    // Calculate Back buffer count
    uint32_t preferredImageCount = 3;
    m_imageCount		 = MIN(surfaceCapabilities.maxImageCount, preferredImageCount);
    m_imageCount		 = MAX(surfaceCapabilities.minImageCount, preferredImageCount);

    VkSurfaceTransformFlagBitsKHR preTransform = (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfaceCapabilities.currentTransform;

    VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    {
	createInfo.surface	    = m_surface;
	createInfo.minImageCount    = m_imageCount;
	createInfo.imageFormat	    = surfaceFormat.format;
	createInfo.imageColorSpace  = surfaceFormat.colorSpace;
	createInfo.imageExtent	    = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage	    = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform	    = preTransform;
	createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode	    = presentMode;
	createInfo.clipped	    = VK_TRUE;
	Vulkan::AddFullscreenExclusiveInfo(createInfo.pNext, m_window);
    }

    // Create swapchain
    if(vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
	LOG_CORE_CRITICAL("Failed to create swapchain");
	return;
    }

    LOG_CORE_INFO("Vulkan Swapchain Created! [Image Count: {0}, Extent: ({1}, {2}), Format: {3}, Color Space: {4}]", m_imageCount, extent.width, extent.height, (uint32_t) surfaceFormat.format, (uint32_t) surfaceFormat.colorSpace);

    // Create images
    VkImage imagesVk[MAX_IMAGE_COUNT];
    {
	VulkanUtilities::checkResult(vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_imageCount, nullptr));
	if(m_imageCount > MAX_IMAGE_COUNT)
	{
	    LOG_CORE_CRITICAL("Swapchain image count higher than supported maximum");
	    return;
	}
	VulkanUtilities::checkResult(vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_imageCount, imagesVk));

	ImageCreateInfo imageCreateInfo = {};
	{
	    imageCreateInfo.Width	= extent.width;
	    imageCreateInfo.Height	= extent.height;
	    imageCreateInfo.Depth	= 1;
	    imageCreateInfo.Layers	= 1;
	    imageCreateInfo.Levels	= 1;
	    imageCreateInfo.Samples	= SampleCount::_1;
	    imageCreateInfo.ImageType	= ImageType::_2D;
	    imageCreateInfo.Format	= static_cast<Format>(surfaceFormat.format);
	    imageCreateInfo.CreateFlags = {};
	    imageCreateInfo.UsageFlags	= ImageUsageFlags::TRANSFER_DST_BIT | ImageUsageFlags::RW_TEXTURE_BIT | ImageUsageFlags::COLOR_ATTACHMENT_BIT | ImageUsageFlags::TEXTURE_BIT;
	}

	for(uint32_t i = 0; i < m_imageCount; i++)
	{
	    auto* memory = m_imageMemoryPool.Alloc();
	    ASSERT(memory);

	    m_images[i] = new(memory) VulkanImage(imagesVk[i], nullptr, imageCreateInfo);
	}
    }

    // Create Synchronization objects
    {
	for(int i = 0; i < BACKBUFFER_COUNT; ++i)
	{
	    VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	    {
		VulkanUtilities::checkResult(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_acquireSemaphores[i]));
		VulkanUtilities::checkResult(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_presentSemaphores[i]));
		char acquireName[32];
		snprintf(acquireName, 32, "Acquire Semaphore %d", i);
		Vulkan::SetResourceName(m_device, VK_OBJECT_TYPE_SEMAPHORE, (uint64_t) m_acquireSemaphores[i], acquireName);
		snprintf(acquireName, 32, "Present Semaphore %d", i);
		Vulkan::SetResourceName(m_device, VK_OBJECT_TYPE_SEMAPHORE, (uint64_t) m_presentSemaphores[i], acquireName);
	    }

	    VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	    {
		VulkanUtilities::checkResult(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_cmdBufferExecutedFences[i]));
	    }
	}
    }

    m_imageFormat	= VulkanUtilities::Translate(surfaceFormat.format);
    m_extent		= { extent.width, extent.height };
    m_currentImageIndex = AcquireNextImageIndex();
}

uint32_t VulkanSwapchain::AcquireNextImageIndex()
{
    VulkanUtilities::checkResult(vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_acquireSemaphores[m_currentSemaphoreIndex], VK_NULL_HANDLE, &m_currentImageIndex));

    m_previousSemaphoreIndex = m_currentSemaphoreIndex;
    m_currentSemaphoreIndex++;
    m_currentSemaphoreIndex = m_currentSemaphoreIndex % m_imageCount;

    VulkanUtilities::checkResult(vkWaitForFences(m_device, 1, &m_cmdBufferExecutedFences[m_currentSemaphoreIndex], VK_TRUE, UINT32_MAX));
    VulkanUtilities::checkResult(vkResetFences(m_device, 1, &m_cmdBufferExecutedFences[m_currentSemaphoreIndex]));

    return m_currentImageIndex;
}

void VulkanSwapchain::Present(Semaphore* waitSemaphore, uint64_t semaphoreWaitValue, Semaphore* signalSemaphore,
			      uint64_t semaphoreSignalValue, Command* command)
{
    const uint32_t resIdx = m_frameIndex % BACKBUFFER_COUNT;

    auto* presentQueue = static_cast<VkQueue>(m_presentQueue->GetNativeHandle());

    {
	VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	auto* waitVkSemaphore		      = static_cast<VkSemaphore>(waitSemaphore->GetNativeHandle());

	uint64_t dummyValue = 0;

	VkTimelineSemaphoreSubmitInfo timelineSubmitInfo { VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
	{
	    timelineSubmitInfo.waitSemaphoreValueCount	 = 1;
	    timelineSubmitInfo.pWaitSemaphoreValues	 = &semaphoreWaitValue;
	    timelineSubmitInfo.signalSemaphoreValueCount = 1;
	    timelineSubmitInfo.pSignalSemaphoreValues	 = &dummyValue;
	}

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	{
	    submitInfo.pNext		    = &timelineSubmitInfo;
	    submitInfo.waitSemaphoreCount   = 1;
	    submitInfo.pWaitSemaphores	    = &waitVkSemaphore;
	    submitInfo.pWaitDstStageMask    = &waitDstStageMask;
	    submitInfo.commandBufferCount   = 0;
	    submitInfo.pCommandBuffers	    = nullptr;
	    submitInfo.signalSemaphoreCount = 1;
	    submitInfo.pSignalSemaphores    = &m_presentSemaphores[resIdx];
	}

	//VulkanUtilities::checkResult(vkQueueSubmit(presentQueue, 1, &submitInfo, VK_NULL_HANDLE));
    }

    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    {
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = &m_presentSemaphores[resIdx];
	presentInfo.swapchainCount     = 1;
	presentInfo.pSwapchains	       = &m_swapchain;
	presentInfo.pImageIndices      = &m_currentImageIndex;
    }

    VulkanUtilities::checkResult(vkQueuePresentKHR(presentQueue, &presentInfo));

    m_currentImageIndex = AcquireNextImageIndex();

    {
	uint64_t dummyValue = 0;

	VkTimelineSemaphoreSubmitInfo timelineSubmitInfo { VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
	{
	    timelineSubmitInfo.waitSemaphoreValueCount	 = 1;
	    timelineSubmitInfo.pWaitSemaphoreValues	 = &dummyValue;
	    timelineSubmitInfo.signalSemaphoreValueCount = 1;
	    timelineSubmitInfo.pSignalSemaphoreValues	 = &semaphoreSignalValue;
	}

	auto* signalVkSemaphore		     = static_cast<VkSemaphore>(signalSemaphore->GetNativeHandle());
	VkPipelineStageFlags submitWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submit_info2 = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	{
	    submit_info2.pNext		      = &timelineSubmitInfo;
	    submit_info2.waitSemaphoreCount   = 1;
	    submit_info2.pWaitSemaphores      = &m_acquireSemaphores[resIdx];
	    submit_info2.pWaitDstStageMask    = &submitWaitStage;
	    submit_info2.commandBufferCount   = 0;
	    submit_info2.pCommandBuffers      = nullptr;
	    submit_info2.signalSemaphoreCount = 1;
	    submit_info2.pSignalSemaphores    = &signalVkSemaphore;
	}

	VulkanUtilities::checkResult(vkQueueSubmit(presentQueue, 1, &submit_info2, VK_NULL_HANDLE));
    }
    m_frameIndex++;
}

void VulkanSwapchain::Destroy()
{
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    for(uint32_t i = 0; i < BACKBUFFER_COUNT; ++i)
    {
	vkDestroySemaphore(m_device, m_acquireSemaphores[i], nullptr);
	vkDestroySemaphore(m_device, m_presentSemaphores[i], nullptr);
    }

    for(uint32_t i = 0; i < m_imageCount; i++)
    {
	m_images[i]->~VulkanImage();
    }
}

void VulkanSwapchain::Resize(uint32_t width, uint32_t height, Window* window, PresentMode presentMode)
{
    m_window	  = window;
    m_presentMode = presentMode;
    Resize(width, height, true);
}

void VulkanSwapchain::Resize(uint32_t width, uint32_t height, bool acquireImage)
{
    vkDeviceWaitIdle(m_device);
    Destroy();
    Create(width, height);
}

uint32_t VulkanSwapchain::GetCurrentImageIndex()
{
    return m_currentImageIndex;
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

PresentMode VulkanSwapchain::GetPresentMode() const
{
    return m_presentMode;
}

void* VulkanSwapchain::GetNativeHandle() const
{
    return m_swapchain;
}