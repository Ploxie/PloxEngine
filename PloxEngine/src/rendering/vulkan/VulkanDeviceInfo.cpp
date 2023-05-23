//
// Created by Ploxie on 2023-05-14.
//
#include "VulkanDeviceInfo.h"
#include "core/Assert.h"
#include "core/Logger.h"
#include "volk.h"
#include "VulkanUtilities.h"

VulkanDeviceInfo::VulkanDeviceInfo(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    : m_physicalDevice(physicalDevice)
{
    // Query Extensions
    {
	uint32_t extensionCount = 0;
	VulkanUtilities::checkResult(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr));
	ASSERT(extensionCount > 0);
	m_extensionProperties.resize(extensionCount);
	VulkanUtilities::checkResult(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, m_extensionProperties.data()));
    }

    // Query Properties
    {
	vkGetPhysicalDeviceProperties(physicalDevice, &m_properties);
    }

    // Query Features
    {
	bool dynamicRenderingExtensionPresent = IsExtensionPresent(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);

	m_dynamicRenderingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR };
	m_vulkan12Features	   = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	m_features2		   = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };

	m_vulkan12Features.pNext = dynamicRenderingExtensionPresent ? &m_dynamicRenderingFeatures : nullptr;
	m_features2.pNext	 = &m_vulkan12Features;

	vkGetPhysicalDeviceFeatures2(physicalDevice, &m_features2);
	m_features = m_features2.features;
    }

    // Query Queues
    {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	eastl::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	for(int queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
	{
	    auto& queueFamily = queueFamilies[queueFamilyIndex];
	    if(queueFamily.queueCount > 0)
	    {
		if(((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0))
		{
		    m_graphicsFamilyIndex = queueFamilyIndex;
		}

		if(((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) && queueFamilyIndex != m_graphicsFamilyIndex)
		{
		    m_computeFamilyIndex = queueFamilyIndex;
		}

		if(((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) && queueFamilyIndex != m_graphicsFamilyIndex && queueFamilyIndex != m_computeFamilyIndex)
		{
		    m_transferFamilyIndex = queueFamilyIndex;
		}
	    }
	}

	// Default to graphics family
	m_computeFamilyIndex  = m_computeFamilyIndex == -1 ? m_graphicsFamilyIndex : m_computeFamilyIndex;
	m_transferFamilyIndex = m_transferFamilyIndex == -1 ? m_graphicsFamilyIndex : m_transferFamilyIndex;
    }

    // Query Surface Support
    {
	VkBool32 graphicsFamilyPresentable = VK_FALSE;
	VkBool32 computeFamilyPresentable  = VK_FALSE;

	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, GetGraphicsQueueFamilyIndex(), surface, &graphicsFamilyPresentable);
	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, GetComputeQueueFamilyIndex(), surface, &computeFamilyPresentable);

	m_graphicsFamilyPresentable = graphicsFamilyPresentable == VK_TRUE;
	m_computeFamilyPresentable  = computeFamilyPresentable == VK_TRUE;
    }
}

bool VulkanDeviceInfo::HasFeatures(VkPhysicalDeviceFeatures features) const
{
    auto elements  = ConvertToElementArray<VkPhysicalDeviceFeatures, VkBool32>(m_features);
    auto elements2 = ConvertToElementArray<VkPhysicalDeviceFeatures, VkBool32>(features);
    for(size_t i = 0; i < elements.size(); i++)
    {
	if(elements[i] == VK_TRUE)
	{
	    continue;
	}
	if(elements2[i] == VK_TRUE)
	{
	    return false;
	}
    }

    return true;
}
bool VulkanDeviceInfo::HasFeatures(VkPhysicalDeviceVulkan12Features features) const
{
    auto elements  = ConvertToElementArray<VkPhysicalDeviceVulkan12Features, VkBool32>(m_vulkan12Features);
    auto elements2 = ConvertToElementArray<VkPhysicalDeviceVulkan12Features, VkBool32>(features);
    for(size_t i = 2; i < elements.size(); i++) // i = 2, because we need to skip sType & pNext
    {
	if(elements[i] == VK_TRUE)
	{
	    continue;
	}
	if(elements2[i] == VK_TRUE)
	{
	    return false;
	}
    }

    return true;
}
bool VulkanDeviceInfo::HasFeatures(VkPhysicalDeviceDynamicRenderingFeatures features) const
{
    auto elements  = ConvertToElementArray<VkPhysicalDeviceDynamicRenderingFeatures, VkBool32>(m_dynamicRenderingFeatures);
    auto elements2 = ConvertToElementArray<VkPhysicalDeviceDynamicRenderingFeatures, VkBool32>(features);
    for(size_t i = 2; i < elements.size(); i++) // i = 2, because we need to skip sType & pNext
    {
	if(elements[i] == VK_TRUE)
	{
	    continue;
	}
	if(elements2[i] == VK_TRUE)
	{
	    return false;
	}
    }

    return true;
}

bool VulkanDeviceInfo::AddExtension(const char* extensionName)
{
    if(!IsExtensionPresent(extensionName))
    {
	LOG_WARN("Physical Device extension '{0}' was not found!", extensionName);
	return false;
    }

    m_extensionNames.push_back(extensionName);

    return true;
}

bool VulkanDeviceInfo::IsExtensionPresent(const char* extensionName) const
{
    for(auto extension: m_extensionProperties)
    {
	if(strcmp(extension.extensionName, extensionName) == 0)
	{
	    return true;
	}
    }

    return false;
}

const eastl::vector<const char*>& VulkanDeviceInfo::GetExtensionNames() const
{
    return m_extensionNames;
}

VkPhysicalDevice VulkanDeviceInfo::GetPhysicalDevice()
{
    return m_physicalDevice;
}
VkPhysicalDeviceProperties VulkanDeviceInfo::GetProperties() const
{
    return m_properties;
}

const int VulkanDeviceInfo::GetGraphicsQueueFamilyIndex() const
{
    return m_graphicsFamilyIndex;
}

const int VulkanDeviceInfo::GetComputeQueueFamilyIndex() const
{
    return m_computeFamilyIndex;
}
const int VulkanDeviceInfo::GetTransferQueueFamilyIndex() const
{
    return m_transferFamilyIndex;
}

const bool VulkanDeviceInfo::IsGraphicsQueuePresentable() const
{
    return m_graphicsFamilyPresentable;
}
const bool VulkanDeviceInfo::IsComputeQueuePresentable() const
{
    return m_computeFamilyPresentable;
}
