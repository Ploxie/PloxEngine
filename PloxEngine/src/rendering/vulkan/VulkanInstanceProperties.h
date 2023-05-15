//
// Created by Ploxie on 2023-05-14.
//

#pragma once
#include "eastl/vector.h"
#include "vulkan/vulkan.h"

class VulkanInstanceProperties
{
public:
    explicit VulkanInstanceProperties();

    bool AddLayer(const char* layerName);
    bool AddExtension(const char* extensionName);

    bool IsLayerPresent(const char* layerName) const;
    bool IsExtensionPresent(const char* extensionName) const;

    const eastl::vector<const char*>& GetLayers() const;
    const eastl::vector<const char*>& GetExtensions() const;

private:
    eastl::vector<VkLayerProperties> m_layerProperties;
    eastl::vector<VkExtensionProperties> m_extensionProperties;
    eastl::vector<const char*> m_layerNames;
    eastl::vector<const char*> m_extensionNames;
};