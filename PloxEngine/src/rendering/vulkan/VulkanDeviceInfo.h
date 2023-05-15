//
// Created by Ploxie on 2023-05-14.
//

#pragma once
#include "eastl/array.h"
#include "eastl/vector.h"
#include "vulkan/vulkan.h"

class VulkanDeviceInfo
{
public:
    VulkanDeviceInfo() = default;
    explicit VulkanDeviceInfo(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

    bool HasFeatures(VkPhysicalDeviceFeatures features) const;
    bool HasFeatures(VkPhysicalDeviceVulkan12Features features) const;
    bool HasFeatures(VkPhysicalDeviceDynamicRenderingFeatures features) const;

    bool AddExtension(const char* extensionName);
    bool IsExtensionPresent(const char* extensionName) const;

    const eastl::vector<const char*>& GetExtensionNames() const;

    VkPhysicalDevice GetPhysicalDevice();
    VkPhysicalDeviceProperties GetProperties() const;

    const int GetGraphicsQueueFamilyIndex() const;
    const int GetComputeQueueFamilyIndex() const;
    const int GetTransferQueueFamilyIndex() const;

    const bool IsGraphicsQueuePresentable() const;
    const bool IsComputeQueuePresentable() const;

private:
    VkPhysicalDevice m_physicalDevice	    = {};
    VkPhysicalDeviceProperties m_properties = {};

    VkPhysicalDeviceFeatures m_features					= {};
    VkPhysicalDeviceFeatures2 m_features2				= {};
    VkPhysicalDeviceVulkan12Features m_vulkan12Features			= {};
    VkPhysicalDeviceDynamicRenderingFeatures m_dynamicRenderingFeatures = {};

    int m_graphicsFamilyIndex	     = -1;
    int m_computeFamilyIndex	     = -1;
    int m_transferFamilyIndex	     = -1;
    bool m_graphicsFamilyPresentable = false;
    bool m_computeFamilyPresentable  = false;

    eastl::vector<VkExtensionProperties> m_extensionProperties;
    eastl::vector<const char*> m_extensionNames;

private:
    template<typename T, typename C>
    eastl::array<C, sizeof(T) / sizeof(C)> ConvertToElementArray(const T& object) const;
};

template<typename T, typename C>
eastl::array<C, sizeof(T) / sizeof(C)> VulkanDeviceInfo::ConvertToElementArray(const T& object) const
{
    eastl::array<C, sizeof(T) / sizeof(C)> elements = {};

    const C* begin = reinterpret_cast<const C*>(eastl::addressof(object));
    const C* end   = begin + (sizeof(T) / sizeof(C));
    eastl::copy(begin, end, eastl::begin(elements));

    return elements;
}