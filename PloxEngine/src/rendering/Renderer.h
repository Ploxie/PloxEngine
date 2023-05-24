//
// Created by Ploxie on 2023-05-09.
//

#pragma once
#include "GraphicsAdapter.h"
#include "rendering/rendergraph/RenderGraph.h"
#include "types/Semaphore.h"
#include "types/Swapchain.h"

class Window;

class CommandPool;
class Command;
class RenderGraph;
class ResourceViewRegistry;
class GridPass;

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

    Image* m_resultImage;
    ResourceStateData m_resultImageState[1]		   = {};
    Buffer* m_mappableConstantBuffers[2]		   = {};
    DescriptorSetLayout* m_offsetBufferDescriptorSetLayout = nullptr;
    DescriptorSetPool* m_offsetBufferDescriptorSetPool	   = nullptr;
    DescriptorSet* m_offsetBufferDescriptorSets[2]	   = {};
    GridPass* m_gridPass;
    CommandPool* m_commandPool;
    Command* m_command;
    DescriptorSet* m_descriptorSet;
    GraphicsPipeline* m_pipeline;
};
