//
// Created by Ploxie on 2023-05-09.
//

#include "Renderer.h"
#include "platform/window/window.h"

void Renderer::Initialize(Window* window, GraphicsBackendType backend)
{
    bool debugLayer   = true;
    m_graphicsAdapter = GraphicsAdapter::Create(window->GetRawHandle(), debugLayer, backend);
    m_graphicsAdapter->CreateSwapchain(m_graphicsAdapter->GetGraphicsQueue(), window->GetWidth(), window->GetHeight(), window, PresentMode::IMMEDIATE, &m_swapchain);

    m_graphicsAdapter->CreateSemaphore(0, &m_semaphores[0]);
    m_graphicsAdapter->CreateSemaphore(0, &m_semaphores[1]);
    m_graphicsAdapter->CreateSemaphore(0, &m_semaphores[2]);
}

void Renderer::Shutdown()
{
}

void Renderer::Render() noexcept
{
    m_swapchain->Present(m_semaphores[0], m_semaphoreValues[0], m_semaphores[0], m_semaphoreValues[0] + 1);
    m_semaphoreValues[0]++;

    m_frame++;
}

bool Renderer::ActivateFullscreen(Window* window)
{
    return m_graphicsAdapter->ActivateFullscreen(window);
}