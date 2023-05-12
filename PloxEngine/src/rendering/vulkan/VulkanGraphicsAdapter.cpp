//
// Created by Ploxie on 2023-05-09.
//
#include "VulkanGraphicsAdapter.h"
#include "core/assert.h"
#include "core/logger.h"
#include "VulkanSwapchain.h"

#undef CreateSemaphore

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    LOG_CORE_ERROR("Vulkan Validation Layer:");
    LOG_CORE_ERROR("Message ID Name: {0}", pCallbackData->pMessageIdName);
    LOG_CORE_ERROR("Message ID Number: {0}", pCallbackData->messageIdNumber);
    LOG_CORE_ERROR("Message: {0}", pCallbackData->pMessage);

    return VK_FALSE;
}

VulkanGraphicsAdapter::VulkanGraphicsAdapter(void* windowHandle, bool debugLayer)
{
    eastl::vector<const char*> requiredInstanceExtensions;
    Vulkan::AppendPlatformExtensions(requiredInstanceExtensions);
    requiredInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // Create Vulkan Instance
    {
	VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	{
	    appInfo.pApplicationName   = "Vulkan";
	    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	    appInfo.pEngineName	       = "Engine";
	    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	    appInfo.apiVersion	       = VK_API_VERSION_1_3;
	}

	// Validate required extensions
	{
	    unsigned int instanceExtensionCount = 0;
	    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
	    eastl::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount);
	    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions.data());

	    for(auto& requiredInstanceExtension: requiredInstanceExtensions)
	    {
		bool found = false;
		for(auto& instanceExtension: instanceExtensions)
		{
		    if(strcmp(requiredInstanceExtension, instanceExtension.extensionName) == 0)
		    {
			found = true;
			break;
		    }
		}

		if(!found)
		{
		    LOG_CRITICAL("Required extension not found: {0}", requiredInstanceExtension);
		}
	    }
	}

	VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	{
	    createInfo.pApplicationInfo	       = &appInfo;
	    createInfo.enabledLayerCount       = 0;
	    createInfo.ppEnabledLayerNames     = nullptr;
	    createInfo.enabledExtensionCount   = static_cast<unsigned int>(requiredInstanceExtensions.size());
	    createInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();
	}

	if(vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
	{
	    LOG_CRITICAL("Failed to create Vulkan Instance!");
	    return;
	}
    }

    // Create debug callback
    {
	VkDebugUtilsMessengerCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	{
	    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	    createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	    createInfo.pfnUserCallback = DebugCallback;
	}

	auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
	if(func(m_instance, &createInfo, nullptr, &m_debugUtilsMessenger) != VK_SUCCESS)
	{
	    LOG_CRITICAL("Failed to set up Vulkan debug callback");
	    return;
	}
    }

    // Create window surface
    {
	if(Vulkan::CreateWindowSurface(windowHandle, m_instance, &m_surface) != VK_SUCCESS)
	{
	    LOG_CRITICAL("Failed to create window surface");
	    return;
	}
    }

    const char* const requiredDeviceExtensions[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME,
    };
    constexpr size_t REQUIRED_DEVICE_EXTENSION_COUNT = 2;
    bool supportsMemoryBudgetExtension		     = false;

    // Choose Physical Device
    {
	unsigned int physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);

	ASSERT_MSG(physicalDeviceCount > 0, "Failed to find device with Vulkan support");

	eastl::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());

	struct DeviceInfo
	{
	    VkPhysicalDevice physicalDevice;
	    VkPhysicalDeviceProperties properties;
	    VkPhysicalDeviceFeatures features;
	    VkPhysicalDeviceVulkan12Features vulkan12Features;
	    unsigned int graphicsQueueFamily;
	    unsigned int computeQueueFamily;
	    unsigned int transferQueueFamily;
	    bool computeQueuePresentable;
	    bool memoryBudgetExtensionSupported;
	    bool dynamicRenderingExtensionSupported;
	};

	eastl::vector<DeviceInfo> suitableDevices;

	for(auto* physicalDevice: physicalDevices)
	{
	    int graphicsFamilyIndex	       = -1;
	    int computeFamilyIndex	       = -1;
	    int transferFamilyIndex	       = -1;
	    VkBool32 graphicsFamilyPresentable = VK_FALSE;
	    VkBool32 computeFamilyPresentable  = VK_FALSE;

	    // Find queue indices
	    {
		unsigned int queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		eastl::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		for(int queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
		{
		    auto& queueFamily = queueFamilies[queueFamilyIndex];
		    if(queueFamily.queueCount > 0 && ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0))
		    {
			graphicsFamilyIndex = queueFamilyIndex;
		    }
		}

		for(int queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
		{
		    if(queueFamilyIndex == graphicsFamilyIndex)
		    {
			continue;
		    }

		    auto& queueFamily = queueFamilies[queueFamilyIndex];
		    if(queueFamily.queueCount > 0 && ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0))
		    {
			computeFamilyIndex = queueFamilyIndex;
		    }
		}

		for(int queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
		{
		    if(queueFamilyIndex == graphicsFamilyIndex || queueFamilyIndex == computeFamilyIndex)
		    {
			continue;
		    }

		    auto& queueFamily = queueFamilies[queueFamilyIndex];
		    if(queueFamily.queueCount > 0 && ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0))
		    {
			transferFamilyIndex = queueFamilyIndex;
		    }
		}

		if(computeFamilyIndex == -1)
		{
		    computeFamilyIndex = graphicsFamilyIndex;
		}
		if(transferFamilyIndex == -1)
		{
		    transferFamilyIndex = graphicsFamilyIndex;
		}

		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndex, m_surface, &graphicsFamilyPresentable);
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, computeFamilyIndex, m_surface, &computeFamilyPresentable);
	    }

	    bool extensionsSupported		    = false;
	    bool memoryBudgetExtensionSupported	    = false;
	    bool dynamicRenderingExtensionSupported = false;
	    {
		unsigned int availableExtensionsCount = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionsCount, nullptr);
		eastl::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionsCount, availableExtensions.data());

		bool foundDeviceExtensions[REQUIRED_DEVICE_EXTENSION_COUNT] = {};
		for(size_t i = 0; i < availableExtensionsCount; i++)
		{
		    for(size_t j = 0; j < REQUIRED_DEVICE_EXTENSION_COUNT; j++)
		    {
			if(strcmp(availableExtensions[i].extensionName, requiredDeviceExtensions[j]) == 0)
			{
			    foundDeviceExtensions[j] = true;
			}
		    }
		    if(strcmp(availableExtensions[i].extensionName, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME) == 0)
		    {
			memoryBudgetExtensionSupported = true;
		    }
		    else if(strcmp(availableExtensions[i].extensionName, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME) == 0)
		    {
			dynamicRenderingExtensionSupported = true;
		    }
		}

		extensionsSupported = true;
		for(auto e: foundDeviceExtensions)
		{
		    if(!e)
		    {
			extensionsSupported = false;
			break;
		    }
		}
	    }

	    bool swapchainAdequate = false;
	    if(extensionsSupported)
	    {
		unsigned int formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, nullptr);

		unsigned int presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);

		swapchainAdequate = formatCount != 0 && presentModeCount != 0;
	    }

	    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR };
	    VkPhysicalDeviceVulkan12Features vulkan12Features			 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, dynamicRenderingExtensionSupported ? &dynamicRenderingFeatures : nullptr };
	    VkPhysicalDeviceFeatures2 supportedFeatures2			 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &vulkan12Features };

	    vkGetPhysicalDeviceFeatures2(physicalDevice, &supportedFeatures2);

	    VkPhysicalDeviceFeatures supportedFeatures = supportedFeatures2.features;

	    if(graphicsFamilyIndex >= 0 && computeFamilyIndex >= 0 && transferFamilyIndex >= 0 && graphicsFamilyPresentable
	       //&& computeFamilyPresentable
	       && extensionsSupported && swapchainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.textureCompressionBC && supportedFeatures.fragmentStoresAndAtomics && supportedFeatures.independentBlend && supportedFeatures.shaderStorageImageExtendedFormats && supportedFeatures.shaderStorageImageWriteWithoutFormat && supportedFeatures.imageCubeArray && supportedFeatures.geometryShader && vulkan12Features.shaderSampledImageArrayNonUniformIndexing && vulkan12Features.timelineSemaphore && vulkan12Features.imagelessFramebuffer && vulkan12Features.descriptorBindingPartiallyBound
	       //&& vulkan12Features.descriptorBindingUniformBufferUpdateAfterBind
	       && vulkan12Features.descriptorBindingSampledImageUpdateAfterBind && vulkan12Features.descriptorBindingStorageBufferUpdateAfterBind && vulkan12Features.descriptorBindingUniformTexelBufferUpdateAfterBind && vulkan12Features.descriptorBindingUpdateUnusedWhilePending)
	    {
		DeviceInfo deviceInfo = {};
		{
		    deviceInfo.physicalDevice = physicalDevice;
		    vkGetPhysicalDeviceProperties(physicalDevice, &deviceInfo.properties);
		    deviceInfo.features				  = supportedFeatures;
		    deviceInfo.vulkan12Features			  = vulkan12Features;
		    deviceInfo.graphicsQueueFamily		  = graphicsFamilyIndex;
		    deviceInfo.computeQueueFamily		  = computeFamilyIndex;
		    deviceInfo.transferQueueFamily		  = transferFamilyIndex;
		    deviceInfo.computeQueuePresentable		  = computeFamilyPresentable == VK_TRUE;
		    deviceInfo.memoryBudgetExtensionSupported	  = memoryBudgetExtensionSupported;
		    deviceInfo.dynamicRenderingExtensionSupported = dynamicRenderingExtensionSupported && dynamicRenderingFeatures.dynamicRendering == VK_TRUE;

		    suitableDevices.push_back(deviceInfo);
		}
	    }
	}

	LOG_INFO("Found {0} suitable device(s): ", suitableDevices.size());

	size_t deviceIndex = 1;

	if(suitableDevices.empty())
	{
	    LOG_CRITICAL("Failed to find suitable GPU");
	    return;
	}
	for(size_t i = 0; i < suitableDevices.size(); ++i)
	{
	    LOG_INFO("[{0}] {1}", (unsigned) i, suitableDevices[i].properties.deviceName);
	}

	if(suitableDevices.size() == 1)
	{
	    deviceIndex = 0;
	}

	const auto& selectedDevice = suitableDevices[deviceIndex];

	m_physicalDevice = selectedDevice.physicalDevice;

	m_graphicsQueue.m_queue		     = VK_NULL_HANDLE;
	m_graphicsQueue.m_queueType	     = VulkanQueue::QueueType::GRAPHICS;
	m_graphicsQueue.m_timestampValidBits = 0;
	m_graphicsQueue.m_timestampPeriod    = selectedDevice.properties.limits.timestampPeriod;
	m_graphicsQueue.m_presentable	     = true;
	m_graphicsQueue.m_queueFamily	     = selectedDevice.graphicsQueueFamily;

	m_computeQueue.m_queue		    = VK_NULL_HANDLE;
	m_computeQueue.m_queueType	    = VulkanQueue::QueueType::COMPUTE;
	m_computeQueue.m_timestampValidBits = 0;
	m_computeQueue.m_timestampPeriod    = selectedDevice.properties.limits.timestampPeriod;
	m_computeQueue.m_presentable	    = selectedDevice.computeQueuePresentable;
	m_computeQueue.m_queueFamily	    = selectedDevice.computeQueueFamily;

	m_transferQueue.m_queue		     = VK_NULL_HANDLE;
	m_transferQueue.m_queueType	     = VulkanQueue::QueueType::TRANSFER;
	m_transferQueue.m_timestampValidBits = 0;
	m_transferQueue.m_timestampPeriod    = selectedDevice.properties.limits.timestampPeriod;
	m_transferQueue.m_presentable	     = false;
	m_transferQueue.m_queueFamily	     = selectedDevice.transferQueueFamily;

	m_properties = selectedDevice.properties;
	m_features   = selectedDevice.features;

	m_dynamicRenderingExtensionSupport = selectedDevice.dynamicRenderingExtensionSupported;
	supportsMemoryBudgetExtension	   = selectedDevice.memoryBudgetExtensionSupported;
    }

    // Create Logical Device
    {
	unsigned int queueCreateInfoCount = 0;
	VkDeviceQueueCreateInfo queueCreateInfos[3];
	unsigned int uniqueQueueFamilies[3];
	unsigned int uniqueQueueFamilyCount = 0;

	uniqueQueueFamilies[uniqueQueueFamilyCount++] = m_graphicsQueue.GetQueueFamily();

	if(m_computeQueue.GetQueueFamily() != uniqueQueueFamilies[0])
	{
	    uniqueQueueFamilies[uniqueQueueFamilyCount++] = m_computeQueue.GetQueueFamily();
	}
	if(m_transferQueue.GetQueueFamily() != uniqueQueueFamilies[0] && m_transferQueue.GetQueueFamily() != uniqueQueueFamilies[1])
	{
	    uniqueQueueFamilies[uniqueQueueFamilyCount++] = m_transferQueue.GetQueueFamily();
	}

	float queuePriority = 1.0F;
	for(size_t i = 0; i < uniqueQueueFamilyCount; i++)
	{
	    VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	    {
		queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
		queueCreateInfo.queueCount	 = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
	    }
	    queueCreateInfos[queueCreateInfoCount++] = queueCreateInfo;
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	{
	    deviceFeatures.samplerAnisotropy			= VK_TRUE;
	    deviceFeatures.textureCompressionBC			= VK_TRUE;
	    deviceFeatures.independentBlend			= VK_TRUE;
	    deviceFeatures.fragmentStoresAndAtomics		= VK_TRUE;
	    deviceFeatures.shaderStorageImageExtendedFormats	= VK_TRUE;
	    deviceFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
	    deviceFeatures.imageCubeArray			= VK_TRUE;
	    deviceFeatures.geometryShader			= VK_TRUE;
	}

	m_enabledFeatures = deviceFeatures;

	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR };
	dynamicRenderingFeatures.dynamicRendering			     = VK_TRUE;

	VkPhysicalDeviceVulkan12Features vulkan12Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, m_dynamicRenderingExtensionSupport ? &dynamicRenderingFeatures : nullptr };
	{
	    vulkan12Features.timelineSemaphore			       = VK_TRUE;
	    vulkan12Features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
	    vulkan12Features.imagelessFramebuffer		       = VK_TRUE;
	    vulkan12Features.descriptorBindingPartiallyBound	       = VK_TRUE;
	    //vulkan12Features.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
	    vulkan12Features.descriptorBindingSampledImageUpdateAfterBind	= VK_TRUE;
	    vulkan12Features.descriptorBindingStorageImageUpdateAfterBind	= VK_TRUE;
	    vulkan12Features.descriptorBindingStorageBufferUpdateAfterBind	= VK_TRUE;
	    vulkan12Features.descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE;
	    vulkan12Features.descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE;
	    vulkan12Features.descriptorBindingUpdateUnusedWhilePending		= VK_TRUE;
	}

	eastl::vector<const char*> enabledExtensions;
	enabledExtensions.reserve(REQUIRED_DEVICE_EXTENSION_COUNT + 2);
	for(const auto* deviceExtension: requiredDeviceExtensions)
	{
	    enabledExtensions.push_back(deviceExtension);
	}
	if(supportsMemoryBudgetExtension)
	{
	    enabledExtensions.push_back(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
	}
	if(m_dynamicRenderingExtensionSupport)
	{
	    enabledExtensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
	}

	VkDeviceCreateInfo createInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	{
	    createInfo.queueCreateInfoCount    = queueCreateInfoCount;
	    createInfo.pQueueCreateInfos       = queueCreateInfos;
	    createInfo.enabledLayerCount       = 0;
	    createInfo.ppEnabledLayerNames     = nullptr;
	    createInfo.pEnabledFeatures	       = &deviceFeatures;
	    createInfo.enabledExtensionCount   = static_cast<unsigned int>(enabledExtensions.size());
	    createInfo.ppEnabledExtensionNames = enabledExtensions.data();
	}

	if(vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
	{
	    LOG_CRITICAL("Failed to create logical device");
	    return;
	}

	LOG_INFO("Logical Device Created ({0})", m_properties.deviceName);

	vkGetDeviceQueue(m_device, m_graphicsQueue.GetQueueFamily(), 0, m_graphicsQueue.GetQueue());
	vkGetDeviceQueue(m_device, m_computeQueue.GetQueueFamily(), 0, m_computeQueue.GetQueue());
	vkGetDeviceQueue(m_device, m_transferQueue.GetQueueFamily(), 0, m_transferQueue.GetQueue());

	unsigned int queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount, nullptr);
	eastl::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

	m_graphicsQueue.m_timestampValidBits = queueFamilyProperties[m_graphicsQueue.m_queueFamily].timestampValidBits;
	m_computeQueue.m_timestampValidBits  = queueFamilyProperties[m_computeQueue.m_queueFamily].timestampValidBits;
	m_transferQueue.m_timestampValidBits = queueFamilyProperties[m_transferQueue.m_queueFamily].timestampValidBits;
    }
}

VulkanGraphicsAdapter::~VulkanGraphicsAdapter()
{
}

void VulkanGraphicsAdapter::CreateSwapchain(const Queue* presentQueue, unsigned int width, unsigned int height, bool fullscreen, PresentMode presentMode, Swapchain** swapchain)
{
    ASSERT(!m_swapchain);
    ASSERT(width && height);

    Queue* queue = nullptr;
    queue	 = presentQueue == &m_graphicsQueue ? &m_graphicsQueue : queue;
    queue	 = presentQueue == &m_computeQueue ? &m_computeQueue : queue;
    ASSERT(queue);
    *swapchain = m_swapchain = new VulkanSwapchain(m_physicalDevice, m_device, m_surface, queue, width, height, fullscreen, presentMode);
}

void VulkanGraphicsAdapter::CreateSemaphore(uint64_t initialValue, Semaphore** semaphore)
{
    *semaphore = new VulkanSemaphore(m_device, initialValue); // TODO: ALLOCATOR
}

Queue* VulkanGraphicsAdapter::GetGraphicsQueue()
{
    return &m_graphicsQueue;
}

Queue* VulkanGraphicsAdapter::GetComputeQueue()
{
    return &m_computeQueue;
}

Queue* VulkanGraphicsAdapter::GetTransferQueue()
{
    return &m_transferQueue;
}
