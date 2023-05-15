//
// Created by Ploxie on 2023-05-09.
//

#pragma once
#include "GraphicsAdapter.h"
#include "types/Semaphore.h"
#include "types/Swapchain.h"

class Window;

class Renderer
{
public:
    explicit Renderer() = default;
    ~Renderer()		= default;

    void Initialize(Window* window, GraphicsBackendType backend);
    void Shutdown();

    void Render() noexcept;

    bool ActivateFullscreen(Window* window);

private:
    GraphicsAdapter* m_graphicsAdapter;
    Swapchain* m_swapchain;
    Semaphore* m_semaphores[3]	   = {};
    uint64_t m_semaphoreValues[3]  = {};
    unsigned int m_frame	   = 0;
    unsigned int m_swapchainWidth  = 1;
    unsigned int m_swapchainHeight = 1;
};
