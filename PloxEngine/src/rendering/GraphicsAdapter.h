//
// Created by Ploxie on 2023-05-09.
//

#pragma once
#include "rendering/types/DescriptorSet.h"
#include "rendering/types/Swapchain.h"
#include <cstdint>

#undef CreateSemaphore

class Window;
struct GraphicsPipelineCreateInfo;
class GraphicsPipeline;
class CommandPool;
class ImageView;
class DescriptorSetLayout;
class DescriptorSetPool;
struct DescriptorSetLayoutBinding;
class ImageViewCreateInfo;

enum class GraphicsBackendType
{
    VULKAN,
    DX12
};

class GraphicsAdapter
{
public:
    virtual ~GraphicsAdapter() = default;

    static GraphicsAdapter* Create(void* windowHandle, bool debugLayer, GraphicsBackendType backend);
    static void Destroy(const GraphicsAdapter* adapter);

    virtual void CreateGraphicsPipeline(uint32_t count, const GraphicsPipelineCreateInfo* createInfo, GraphicsPipeline** pipelines)				     = 0;
    virtual void CreateCommandPool(const Queue* queue, CommandPool** commandPool)										     = 0;
    virtual void CreateSwapchain(const Queue* presentQueue, unsigned int width, unsigned int height, Window* window, PresentMode presentMode, Swapchain** swapchain) = 0;
    virtual void CreateSemaphore(uint64_t initialValue, Semaphore** semaphore)											     = 0;
    virtual void CreateImageView(const ImageViewCreateInfo* imageViewCreateInfo, ImageView** imageView)								     = 0;
    virtual void CreateImageView(Image* image, ImageView** imageView)												     = 0;
    virtual void CreateDescriptorSetPool(uint32_t maxSets, const DescriptorSetLayout* descriptorSetLayout, DescriptorSetPool** descriptorSetPool)		     = 0;
    virtual void CreateDescriptorSetLayout(uint32_t bindingCount, const DescriptorSetLayoutBinding* bindings, DescriptorSetLayout** descriptorSetLayout)	     = 0;

    virtual bool ActivateFullscreen(Window* window) = 0;

    virtual Queue* GetGraphicsQueue() = 0;
    virtual Queue* GetComputeQueue()  = 0;
    virtual Queue* GetTransferQueue() = 0;
};