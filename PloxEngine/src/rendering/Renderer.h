//
// Created by Ploxie on 2023-05-09.
//

#pragma once
#include "GraphicsAdapter.h"
#include "rendering/rendergraph/RenderGraph.h"
#include "types/Semaphore.h"
#include "types/Swapchain.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

class Window;

class CommandPool;
class Command;
class RenderGraph;
class ResourceViewRegistry;
class RenderView;

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

    RenderGraph* m_renderGraph;
    ResourceViewRegistry* m_viewRegistry;
    RenderView* m_renderView;

    DescriptorSetLayout* m_offsetBufferDescriptorSetLayout = nullptr;
    DescriptorSetPool* m_offsetBufferDescriptorSetPool	   = nullptr;
    DescriptorSet* m_offsetBufferDescriptorSets[2]	   = {};
    Buffer* m_mappableConstantBuffers[2]		   = {};

    //CAMERA
    glm::quat m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 m_position = { 0, 0, 2.0f };
};
