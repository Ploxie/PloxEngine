//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "vulkan/vulkan.h"

namespace VulkanUtilities
{
    VkResult checkResult(VkResult result, const char *errorMsg = nullptr, bool exitOnError = true);
}