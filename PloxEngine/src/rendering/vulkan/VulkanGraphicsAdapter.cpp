//
// Created by Ploxie on 2023-05-09.
//
#include "VulkanGraphicsAdapter.h"
#include "core/assert.h"
#include "core/logger.h"
#include "eastl/string.h"
#include "platform/window/Window.h"
#include "VulkanDeviceInfo.h"
#include "VulkanFrameBufferCache.h"
#include "VulkanRenderPassCache.h"
#include "VulkanSwapchain.h"

#undef CreateSemaphore

static PFN_vkSetDebugUtilsObjectNameEXT s_vkSetDebugUtilsObjectName = {};

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    spdlog::level::level_enum severity = spdlog::level::info;
    if((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
	severity = spdlog::level::warn;
    }
    else if((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
	severity = spdlog::level::err;
    }

    Logger::GetCoreLogger()->log(severity, "{0}\n", pCallbackData->pMessage);

    return VK_FALSE;
}

VulkanGraphicsAdapter::VulkanGraphicsAdapter(void* windowHandle, bool debugLayer)
{
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

	VulkanInstanceProperties instanceProperties;
	instanceProperties.AddExtension(VK_KHR_SURFACE_EXTENSION_NAME);
	instanceProperties.AddExtension(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);
	instanceProperties.AddExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	instanceProperties.AddExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	instanceProperties.AddLayer("VK_LAYER_KHRONOS_validation");
	Vulkan::AddPlatformInstanceExtensions(instanceProperties);

	const VkValidationFeatureEnableEXT featuresRequested[] = { VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT, VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT, VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT };
	VkValidationFeaturesEXT validationFeatures	       = { VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT };
	{
	    validationFeatures.enabledValidationFeatureCount = _countof(featuresRequested);
	    validationFeatures.pEnabledValidationFeatures    = featuresRequested;
	}

	VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	{
	    createInfo.pNext		       = &validationFeatures;
	    createInfo.pApplicationInfo	       = &appInfo;
	    createInfo.enabledLayerCount       = static_cast<unsigned int>(instanceProperties.GetLayers().size());
	    createInfo.ppEnabledLayerNames     = instanceProperties.GetLayers().data();
	    createInfo.enabledExtensionCount   = static_cast<unsigned int>(instanceProperties.GetExtensions().size());
	    createInfo.ppEnabledExtensionNames = instanceProperties.GetExtensions().data();
	}

	if(vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
	{
	    LOG_CORE_CRITICAL("Failed to create Vulkan Instance!");
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
	    LOG_CORE_CRITICAL("Failed to set up Vulkan debug callback");
	    return;
	}
    }

    // Create window surface
    {
	if(Vulkan::CreateWindowSurface(windowHandle, m_instance, &m_surface) != VK_SUCCESS)
	{
	    LOG_CORE_CRITICAL("Failed to create window surface");
	    return;
	}
    }

    VkPhysicalDeviceFeatures requiredFeatures = {};
    {
	requiredFeatures.samplerAnisotropy		      = VK_TRUE;
	requiredFeatures.textureCompressionBC		      = VK_TRUE;
	requiredFeatures.fragmentStoresAndAtomics	      = VK_TRUE;
	requiredFeatures.independentBlend		      = VK_TRUE;
	requiredFeatures.shaderStorageImageExtendedFormats    = VK_TRUE;
	requiredFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
	requiredFeatures.imageCubeArray			      = VK_TRUE;
	requiredFeatures.geometryShader			      = VK_TRUE;
    }

    VkPhysicalDeviceVulkan12Features requiredVulkan12Features = {};
    {
	requiredVulkan12Features.shaderSampledImageArrayNonUniformIndexing	    = VK_TRUE;
	requiredVulkan12Features.timelineSemaphore				    = VK_TRUE;
	requiredVulkan12Features.imagelessFramebuffer				    = VK_TRUE;
	requiredVulkan12Features.descriptorBindingPartiallyBound		    = VK_TRUE;
	requiredVulkan12Features.descriptorBindingSampledImageUpdateAfterBind	    = VK_TRUE;
	requiredVulkan12Features.descriptorBindingStorageBufferUpdateAfterBind	    = VK_TRUE;
	requiredVulkan12Features.descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE;
	requiredVulkan12Features.descriptorBindingUpdateUnusedWhilePending	    = VK_TRUE;
	//requiredVulkan12Features.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    }

    VkPhysicalDeviceDynamicRenderingFeatures requiredDynamicRenderingFeatures = {};
    {
	requiredDynamicRenderingFeatures.dynamicRendering = VK_TRUE;
    }

    VulkanDeviceInfo selectedDevice = {};
    // Choose Physical Device
    {
	eastl::vector<VkPhysicalDevice> physicalDevices;
	{
	    unsigned int physicalDeviceCount = 0;
	    vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);

	    ASSERT_MSG(physicalDeviceCount > 0, "Failed to find device with Vulkan support");

	    physicalDevices.resize(physicalDeviceCount);
	    vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());
	}

	eastl::vector<VulkanDeviceInfo> suitableDevices;

	for(auto* physicalDevice: physicalDevices)
	{
	    VulkanDeviceInfo deviceInfo(physicalDevice, m_surface);

	    // Check if required Extensions are supported
	    {
		if(!deviceInfo.AddExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
		{
		    continue;
		}

		if(!deviceInfo.AddExtension(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME))
		{
		    continue;
		}
	    }

	    // Check if required Features are supported
	    {
		if(!deviceInfo.HasFeatures(requiredFeatures))
		{
		    continue;
		}
		if(!deviceInfo.HasFeatures(requiredVulkan12Features))
		{
		    continue;
		}
		if(!deviceInfo.HasFeatures(requiredDynamicRenderingFeatures))
		{
		    continue;
		}
	    }

	    // Check if required Queue properties are supported
	    {
		if(deviceInfo.GetGraphicsQueueFamilyIndex() < 0 || deviceInfo.GetComputeQueueFamilyIndex() < 0 || deviceInfo.GetTransferQueueFamilyIndex() < 0)
		{
		    continue;
		}
		if(!deviceInfo.IsGraphicsQueuePresentable())
		{
		    continue;
		}
	    }

	    // Check if swapchain is adequate
	    {
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, nullptr);

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);

		if(formatCount == 0 || presentModeCount == 0)
		{
		    continue;
		}
	    }

	    suitableDevices.push_back(deviceInfo);
	}

	// Select suitable Physical Device
	{
	    size_t deviceIndex = 1;

	    if(suitableDevices.empty())
	    {
		LOG_CORE_CRITICAL("Failed to find suitable GPU");
		return;
	    }

	    LOG_CORE_INFO("Found {0} suitable device(s): ", suitableDevices.size());

	    for(size_t i = 0; i < suitableDevices.size(); ++i)
	    {
		LOG_CORE_INFO("\t[{0}] {1}", (unsigned) i, suitableDevices[i].GetProperties().deviceName);
	    }

	    if(suitableDevices.size() == 1)
	    {
		deviceIndex = 0;
	    }

	    selectedDevice = suitableDevices[deviceIndex];
	}

	m_physicalDevice = selectedDevice.GetPhysicalDevice();

	// Populate Queue Info
	{
	    m_graphicsQueue.m_queue		 = VK_NULL_HANDLE;
	    m_graphicsQueue.m_queueType		 = VulkanQueue::QueueType::GRAPHICS;
	    m_graphicsQueue.m_timestampValidBits = 0;
	    m_graphicsQueue.m_timestampPeriod	 = selectedDevice.GetProperties().limits.timestampPeriod;
	    m_graphicsQueue.m_presentable	 = true;
	    m_graphicsQueue.m_queueFamily	 = selectedDevice.GetGraphicsQueueFamilyIndex();

	    m_computeQueue.m_queue		= VK_NULL_HANDLE;
	    m_computeQueue.m_queueType		= VulkanQueue::QueueType::COMPUTE;
	    m_computeQueue.m_timestampValidBits = 0;
	    m_computeQueue.m_timestampPeriod	= selectedDevice.GetProperties().limits.timestampPeriod;
	    m_computeQueue.m_presentable	= selectedDevice.IsComputeQueuePresentable();
	    m_computeQueue.m_queueFamily	= selectedDevice.GetComputeQueueFamilyIndex();

	    m_transferQueue.m_queue		 = VK_NULL_HANDLE;
	    m_transferQueue.m_queueType		 = VulkanQueue::QueueType::TRANSFER;
	    m_transferQueue.m_timestampValidBits = 0;
	    m_transferQueue.m_timestampPeriod	 = selectedDevice.GetProperties().limits.timestampPeriod;
	    m_transferQueue.m_presentable	 = false;
	    m_transferQueue.m_queueFamily	 = selectedDevice.GetTransferQueueFamilyIndex();
	}
    }

    // Create Logical Device
    {
	unsigned int queueCreateInfoCount = 0;
	VkDeviceQueueCreateInfo queueCreateInfos[3];
	// Create Queue Info
	{
	    unsigned int uniqueQueueFamilies[3];
	    unsigned int uniqueQueueFamilyCount = 0;

	    // Find unique Queue Families
	    {
		uniqueQueueFamilies[uniqueQueueFamilyCount++] = m_graphicsQueue.GetQueueFamily();

		if(m_computeQueue.GetQueueFamily() != uniqueQueueFamilies[0])
		{
		    uniqueQueueFamilies[uniqueQueueFamilyCount++] = m_computeQueue.GetQueueFamily();
		}
		if(m_transferQueue.GetQueueFamily() != uniqueQueueFamilies[0] && m_transferQueue.GetQueueFamily() != uniqueQueueFamilies[1])
		{
		    uniqueQueueFamilies[uniqueQueueFamilyCount++] = m_transferQueue.GetQueueFamily();
		}
	    }

	    // Create Queue Create Infos
	    {
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
	    }
	}

	// Enabled Non-Required Extensions
	{
	    m_fullscreenExclusiveSupported     = selectedDevice.AddExtension("VK_EXT_full_screen_exclusive");
	    m_dynamicRenderingExtensionSupport = selectedDevice.HasFeatures(requiredDynamicRenderingFeatures);
	}

	// Enabled features
	auto deviceFeatures	      = requiredFeatures;
	auto dynamicRenderingFeatures = requiredDynamicRenderingFeatures;
	{
	    dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	}
	auto vulkan12Features = requiredVulkan12Features;
	{
	    vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	    vulkan12Features.pNext = dynamicRenderingFeatures.dynamicRendering ? &dynamicRenderingFeatures : nullptr;
	}

	VkPhysicalDeviceFeatures2 deviceFeatures2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
	{
	    deviceFeatures2.features = deviceFeatures;
	    deviceFeatures2.pNext    = &vulkan12Features;
	}

	const auto& enabledExtensions = selectedDevice.GetExtensionNames();
	VkDeviceCreateInfo createInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	{
	    createInfo.pNext		       = &deviceFeatures2;
	    createInfo.queueCreateInfoCount    = queueCreateInfoCount;
	    createInfo.pQueueCreateInfos       = queueCreateInfos;
	    createInfo.enabledLayerCount       = 0;
	    createInfo.ppEnabledLayerNames     = nullptr;
	    createInfo.pEnabledFeatures	       = nullptr;
	    createInfo.enabledExtensionCount   = static_cast<unsigned int>(enabledExtensions.size());
	    createInfo.ppEnabledExtensionNames = enabledExtensions.data();
	}

	if(vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
	{
	    LOG_CORE_CRITICAL("Failed to create logical device");
	    return;
	}

	LOG_CORE_INFO("Logical Device Created ({0})", selectedDevice.GetProperties().deviceName);

	s_vkSetDebugUtilsObjectName = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetDeviceProcAddr(m_device, "vkSetDebugUtilsObjectNameEXT");

	Vulkan::InitializePlatform(m_instance, m_device);

	// Get Device Queue Info
	{
	    vkGetDeviceQueue(m_device, m_graphicsQueue.GetQueueFamily(), 0, m_graphicsQueue.GetQueue());
	    vkGetDeviceQueue(m_device, m_computeQueue.GetQueueFamily(), 0, m_computeQueue.GetQueue());
	    vkGetDeviceQueue(m_device, m_transferQueue.GetQueueFamily(), 0, m_transferQueue.GetQueue());

	    Vulkan::SetResourceName(m_device, VK_OBJECT_TYPE_QUEUE, (uint64_t) *m_graphicsQueue.GetQueue(), "Graphics Queue");
	    Vulkan::SetResourceName(m_device, VK_OBJECT_TYPE_QUEUE, (uint64_t) *m_computeQueue.GetQueue(), "Compute Queue");
	    Vulkan::SetResourceName(m_device, VK_OBJECT_TYPE_QUEUE, (uint64_t) *m_transferQueue.GetQueue(), "Transfer Queue");

	    unsigned int queueFamilyPropertyCount = 0;
	    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount, nullptr);
	    eastl::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
	    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

	    m_graphicsQueue.m_timestampValidBits = queueFamilyProperties[m_graphicsQueue.m_queueFamily].timestampValidBits;
	    m_computeQueue.m_timestampValidBits	 = queueFamilyProperties[m_computeQueue.m_queueFamily].timestampValidBits;
	    m_transferQueue.m_timestampValidBits = queueFamilyProperties[m_transferQueue.m_queueFamily].timestampValidBits;
	}
    }

    m_renderPassCache  = new VulkanRenderPassCache(m_device);
    m_frameBufferCache = new VulkanFrameBufferCache(m_device);
}

VulkanGraphicsAdapter::~VulkanGraphicsAdapter()
{
    delete m_renderPassCache;
}

void VulkanGraphicsAdapter::CreateSwapchain(const Queue* presentQueue, unsigned int width, unsigned int height, Window* window, PresentMode presentMode, Swapchain** swapchain)
{
    ASSERT(!m_swapchain);
    ASSERT(width && height);

    Queue* queue = nullptr;

    queue = presentQueue == &m_graphicsQueue ? &m_graphicsQueue : queue;
    queue = presentQueue == &m_computeQueue ? &m_computeQueue : queue;
    ASSERT(queue);

    *swapchain = m_swapchain = new VulkanSwapchain(m_physicalDevice, m_device, m_surface, queue, width, height, window, presentMode);
}

void VulkanGraphicsAdapter::CreateSemaphore(uint64_t initialValue, Semaphore** semaphore)
{
    *semaphore = new VulkanSemaphore(m_device, initialValue); // TODO: ALLOCATOR
}

bool VulkanGraphicsAdapter::ActivateFullscreen(Window* window)
{
    if(m_swapchain == nullptr || !m_fullscreenExclusiveSupported)
    {
	return false;
    }

    m_swapchain->Resize(window->GetWidth(), window->GetHeight(), window, m_swapchain->GetPresentMode());
    return Vulkan::ActivateFullscreen(window, m_swapchain);
}

VkDevice& VulkanGraphicsAdapter::GetDevice()
{
    return m_device;
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

VkRenderPass VulkanGraphicsAdapter::GetRenderPass(const VulkanRenderPassDescription& renderPassDescription)
{
    return m_renderPassCache->GetRenderPass(renderPassDescription);
}

VkFramebuffer VulkanGraphicsAdapter::GetFrameBuffer(const VulkanFrameBufferDescription& frameBufferDescription)
{
    return m_frameBufferCache->GetFrameBuffer(frameBufferDescription);
}

bool VulkanGraphicsAdapter::IsDynamicRenderingExtensionSupported()
{
    return m_dynamicRenderingExtensionSupport;
}

void Vulkan::SetResourceName(VkDevice device, VkObjectType type, uint64_t handle, const char* name)
{
    if(handle == 0 || name == nullptr)
    {
	return;
    }

    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    VkDebugUtilsObjectNameInfoEXT nameInfo = {};
    nameInfo.sType			   = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    nameInfo.objectType			   = type;
    nameInfo.objectHandle		   = handle;
    nameInfo.pObjectName		   = name;
    s_vkSetDebugUtilsObjectName(device, &nameInfo);
}