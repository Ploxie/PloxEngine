//
// Created by Ploxie on 2023-05-11.
//
#include "VulkanUtilities.h"
#include "core/Logger.h"

VkResult VulkanUtilities::checkResult(VkResult result, const char *errorMsg, bool exitOnError)
{
    if(result == VK_SUCCESS)
    {
	return result;
    }

    constexpr size_t maxErrorMsgLength = 256;
    constexpr size_t maxResultLength   = 128;
    constexpr size_t msgBufferLength   = maxErrorMsgLength + maxResultLength;

    char msgBuffer[msgBufferLength];
    msgBuffer[0] = '\0';

    if(errorMsg)
    {
	LOG_CORE_CRITICAL("{0}", msgBuffer);
    }

    bool error = false;

    switch(result)
    {
	case VK_SUCCESS:
	    break;
	case VK_NOT_READY:
	    break;
	case VK_TIMEOUT:
	    break;
	case VK_EVENT_SET:
	    break;
	case VK_EVENT_RESET:
	    break;
	case VK_INCOMPLETE:
	    break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
	    strcat_s(msgBuffer, " VK_ERROR_OUT_OF_HOST_MEMORY\n");
	    error = true;
	    break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
	    strcat_s(msgBuffer, " VK_ERROR_OUT_OF_DEVICE_MEMORY\n");
	    error = true;
	    break;
	case VK_ERROR_INITIALIZATION_FAILED:
	    strcat_s(msgBuffer, " VK_ERROR_INITIALIZATION_FAILED\n");
	    error = true;
	    break;
	case VK_ERROR_DEVICE_LOST:
	    strcat_s(msgBuffer, " VK_ERROR_DEVICE_LOST\n");
	    error = true;
	    break;
	case VK_ERROR_MEMORY_MAP_FAILED:
	    strcat_s(msgBuffer, " VK_ERROR_MEMORY_MAP_FAILED\n");
	    error = true;
	    break;
	case VK_ERROR_LAYER_NOT_PRESENT:
	    strcat_s(msgBuffer, " VK_ERROR_LAYER_NOT_PRESENT\n");
	    error = true;
	    break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
	    strcat_s(msgBuffer, " VK_ERROR_EXTENSION_NOT_PRESENT\n");
	    error = true;
	    break;
	case VK_ERROR_FEATURE_NOT_PRESENT:
	    strcat_s(msgBuffer, " VK_ERROR_FEATURE_NOT_PRESENT\n");
	    error = true;
	    break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
	    strcat_s(msgBuffer, " VK_ERROR_INCOMPATIBLE_DRIVER\n");
	    error = true;
	    break;
	case VK_ERROR_TOO_MANY_OBJECTS:
	    strcat_s(msgBuffer, " VK_ERROR_TOO_MANY_OBJECTS\n");
	    error = true;
	    break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
	    strcat_s(msgBuffer, " VK_ERROR_FORMAT_NOT_SUPPORTED\n");
	    error = true;
	    break;
	case VK_ERROR_FRAGMENTED_POOL:
	    strcat_s(msgBuffer, " VK_ERROR_FRAGMENTED_POOL\n");
	    error = true;
	    break;
	case VK_ERROR_UNKNOWN:
	    strcat_s(msgBuffer, " VK_ERROR_UNKNOWN\n");
	    error = true;
	    break;
	case VK_ERROR_OUT_OF_POOL_MEMORY:
	    strcat_s(msgBuffer, " VK_ERROR_OUT_OF_POOL_MEMORY\n");
	    error = true;
	    break;
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
	    strcat_s(msgBuffer, " VK_ERROR_INVALID_EXTERNAL_HANDLE\n");
	    error = true;
	    break;
	case VK_ERROR_FRAGMENTATION:
	    strcat_s(msgBuffer, " VK_ERROR_FRAGMENTATION\n");
	    error = true;
	    break;
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
	    strcat_s(msgBuffer, " VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS\n");
	    error = true;
	    break;
	case VK_ERROR_SURFACE_LOST_KHR:
	    strcat_s(msgBuffer, " VK_ERROR_SURFACE_LOST_KHR\n");
	    error = true;
	    break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
	    strcat_s(msgBuffer, " VK_ERROR_NATIVE_WINDOW_IN_USE_KHR\n");
	    error = true;
	    break;
	case VK_SUBOPTIMAL_KHR:
	    break;
	case VK_ERROR_OUT_OF_DATE_KHR:
	    strcat_s(msgBuffer, " VK_ERROR_OUT_OF_DATE_KHR\n");
	    error = true;
	    break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
	    strcat_s(msgBuffer, " VK_ERROR_INCOMPATIBLE_DISPLAY_KHR\n");
	    error = true;
	    break;
	case VK_ERROR_VALIDATION_FAILED_EXT:
	    strcat_s(msgBuffer, " VK_ERROR_VALIDATION_FAILED_EXT\n");
	    error = true;
	    break;
	case VK_ERROR_INVALID_SHADER_NV:
	    strcat_s(msgBuffer, " VK_ERROR_INVALID_SHADER_NV\n");
	    error = true;
	    break;
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
	    strcat_s(msgBuffer, " VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT\n");
	    error = true;
	    break;
	case VK_ERROR_NOT_PERMITTED_EXT:
	    strcat_s(msgBuffer, " VK_ERROR_NOT_PERMITTED_EXT\n");
	    error = true;
	    break;
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
	    strcat_s(msgBuffer, " VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT\n");
	    error = true;
	    break;
	default:
	    break;
    }

    if(error)
    {
	LOG_ERROR("{0}", msgBuffer);
    }

    return result;
}