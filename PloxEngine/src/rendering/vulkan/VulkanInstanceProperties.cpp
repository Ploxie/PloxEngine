//
// Created by Ploxie on 2023-05-14.
//
#include "VulkanInstanceProperties.h"
#include "core/Assert.h"
#include "core/Logger.h"
#include "VulkanUtilities.h"

VulkanInstanceProperties::VulkanInstanceProperties()
{
    uint32_t layerCount = 0;
    VulkanUtilities::checkResult(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
    ASSERT(layerCount > 0);
    m_layerProperties.resize(layerCount);

    VulkanUtilities::checkResult(vkEnumerateInstanceLayerProperties(&layerCount, m_layerProperties.data()));

    uint32_t extensionCount = 0;
    VulkanUtilities::checkResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
    ASSERT(extensionCount > 0);
    m_extensionProperties.resize(extensionCount);

    VulkanUtilities::checkResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_extensionProperties.data()));
}

bool VulkanInstanceProperties::AddLayer(const char* layerName)
{
    if(!IsLayerPresent(layerName))
    {
	LOG_WARN("Instance layer '{0}' was not found!", layerName);
	return false;
    }

    m_layerNames.push_back(layerName);

    return true;
}
bool VulkanInstanceProperties::AddExtension(const char* extensionName)
{
    if(!IsExtensionPresent(extensionName))
    {
	LOG_WARN("Instance extension '{0}' was not found!", extensionName);
	return false;
    }

    m_extensionNames.push_back(extensionName);

    return true;
}

bool VulkanInstanceProperties::IsLayerPresent(const char* layerName) const
{
    for(auto layer: m_layerProperties)
    {
	if(strcmp(layer.layerName, layerName) == 0)
	{
	    return true;
	}
    }

    return false;
}

bool VulkanInstanceProperties::IsExtensionPresent(const char* extensionName) const
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

const eastl::vector<const char*>& VulkanInstanceProperties::GetLayers() const
{
    return m_layerNames;
}

const eastl::vector<const char*>& VulkanInstanceProperties::GetExtensions() const
{
    return m_extensionNames;
}
