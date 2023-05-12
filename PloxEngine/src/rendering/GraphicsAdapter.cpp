//
// Created by Ploxie on 2023-05-09.
//
#include "GraphicsAdapter.h"
#include "rendering/vulkan/VulkanGraphicsAdapter.h"
#include "core/Logger.h"

GraphicsAdapter* GraphicsAdapter::Create(void* windowHandle, bool debugLayer, GraphicsBackendType backend)
{
	switch (backend)
	{
	case GraphicsBackendType::VULKAN:
		return new VulkanGraphicsAdapter(windowHandle, debugLayer);
	default:
		LOG_CRITICAL("Render backend type {0} not supported!", (int)backend);
		break;
	}
	return nullptr;
}

void GraphicsAdapter::Destroy(const GraphicsAdapter* adapter)
{

}
