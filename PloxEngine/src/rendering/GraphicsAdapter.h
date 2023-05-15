//
// Created by Ploxie on 2023-05-09.
//

#pragma once
#include "types/Queue.h"
#include "types/Semaphore.h"
#include "types/Swapchain.h"

#undef CreateSemaphore

class Window;

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

    virtual void CreateSwapchain(const Queue* presentQueue, unsigned int width, unsigned int height, Window* window, PresentMode presentMode, Swapchain** swapchain) = 0;
    virtual void CreateSemaphore(uint64_t initialValue, Semaphore** semaphore)											     = 0;

    virtual bool ActivateFullscreen(Window* window) = 0;

    virtual Queue* GetGraphicsQueue() = 0;
    virtual Queue* GetComputeQueue()  = 0;
    virtual Queue* GetTransferQueue() = 0;
};