//
// Created by Ploxie on 2023-05-15.
//

#ifdef _WIN32
    #include "core/Assert.h"
    #include "platform/Platform.h"
    #include "rendering/vulkan/VulkanGraphicsAdapter.h"
    #include "rendering/vulkan/VulkanSwapchain.h"
    #include "rendering/vulkan/VulkanUtilities.h"
    #include "vulkan/vulkan.h"
    #include <vulkan/vulkan_win32.h>

static VkInstance s_vulkanInstance = {};
static VkDevice s_vulkanDevice	   = {};

static VkSurfaceFullScreenExclusiveInfoEXT s_surfaceFullscreenExclusiveInfo	      = {};
static VkSurfaceFullScreenExclusiveWin32InfoEXT s_surfaceFullscreenExclusiveWin32Info = {};

static PFN_vkAcquireFullScreenExclusiveModeEXT s_acquireFullscreenExclusiveMode = {};
static PFN_vkReleaseFullScreenExclusiveModeEXT s_releaseFullscreenExclusiveMode = {};

static RECT s_windowedRect    = {};
static LONG s_windowedStyle   = {};
static LONG s_windowedStyleEx = {};

void Vulkan::AddPlatformInstanceExtensions(VulkanInstanceProperties& instanceProperties)
{
    instanceProperties.AddExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
}

VkResult Vulkan::CreateWindowSurface(void* windowHandle, VkInstance instance, VkSurfaceKHR* surface)
{
    VkWin32SurfaceCreateInfoKHR create_info = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    create_info.hinstance		    = static_cast<HINSTANCE>(Platform::GetApplicationInstance());
    create_info.hwnd			    = static_cast<HWND>(windowHandle);

    return vkCreateWin32SurfaceKHR(instance, &create_info, nullptr, surface);
}

void Vulkan::InitializePlatform(VkInstance instance, VkDevice device)
{
    s_vulkanInstance = instance;
    s_vulkanDevice   = device;

    auto func = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr"));

    s_acquireFullscreenExclusiveMode = reinterpret_cast<PFN_vkAcquireFullScreenExclusiveModeEXT>(func(device, "vkAcquireFullScreenExclusiveModeEXT"));
    ASSERT(s_acquireFullscreenExclusiveMode);
    s_releaseFullscreenExclusiveMode = reinterpret_cast<PFN_vkReleaseFullScreenExclusiveModeEXT>(func(device, "vkReleaseFullScreenExclusiveModeEXT"));
    ASSERT(s_releaseFullscreenExclusiveMode);
}

void Vulkan::AddSwapchainWindowInfo(VkSwapchainCreateInfoKHR& swapchainInfo, Window* window)
{
    s_surfaceFullscreenExclusiveWin32Info = { VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT };
    {
	auto* hwnd				       = static_cast<HWND>(window->GetRawHandle());
	s_surfaceFullscreenExclusiveWin32Info.hmonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    }

    s_surfaceFullscreenExclusiveInfo = { VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT, &s_surfaceFullscreenExclusiveWin32Info };
    switch(window->GetWindowMode())
    {
	case WindowMode::WINDOWED: s_surfaceFullscreenExclusiveInfo.fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_DISALLOWED_EXT; break;
	case WindowMode::FULLSCREEN: s_surfaceFullscreenExclusiveInfo.fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT; break;
	case WindowMode::WINDOWED_FULLSCREEN: s_surfaceFullscreenExclusiveInfo.fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_ALLOWED_EXT; break;
    }

    swapchainInfo.pNext = &s_surfaceFullscreenExclusiveInfo;
}

bool Vulkan::ActivateFullscreen(Window* window, VulkanSwapchain* swapchain)
{
    auto* vulkanSwapchain = static_cast<VkSwapchainKHR>(swapchain->GetNativeHandle());
    auto* hwnd		  = static_cast<HWND>(window->GetRawHandle());

    static bool isFullscreen = false;

    if(window->GetWindowMode() == WindowMode::FULLSCREEN)
    {
	if(!isFullscreen)
	{
	    isFullscreen = true;

	    GetWindowRect(hwnd, &s_windowedRect);

	    s_windowedStyle = GetWindowLong(hwnd, GWL_STYLE);
	    SetWindowLong(hwnd, GWL_STYLE, s_windowedStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

	    MONITORINFO monitorInfo;
	    monitorInfo.cbSize = sizeof(monitorInfo);
	    GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitorInfo);

	    SetWindowPos(
		hwnd,
		HWND_NOTOPMOST,
		monitorInfo.rcMonitor.left,
		monitorInfo.rcMonitor.top,
		monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
		monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
		SWP_FRAMECHANGED | SWP_NOACTIVATE);

	    ShowWindow(hwnd, SW_MAXIMIZE);

	    VulkanUtilities::checkResult(s_acquireFullscreenExclusiveMode(s_vulkanDevice, vulkanSwapchain));
	}
	return true;
    }

    if(isFullscreen)
    {
	isFullscreen = false;

	VulkanUtilities::checkResult(s_releaseFullscreenExclusiveMode(s_vulkanDevice, vulkanSwapchain));

	SetWindowLong(hwnd, GWL_STYLE, s_windowedStyle);
	SetWindowPos(
	    hwnd,
	    HWND_NOTOPMOST,
	    s_windowedRect.left,
	    s_windowedRect.top,
	    s_windowedRect.right - s_windowedRect.left,
	    s_windowedRect.bottom - s_windowedRect.top,
	    SWP_FRAMECHANGED | SWP_NOACTIVATE);

	ShowWindow(hwnd, SW_NORMAL);
    }

    return false;
}

#endif