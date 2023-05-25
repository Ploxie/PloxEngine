//
// Created by Ploxie on 2023-05-09.
//

#include "Renderer.h"
#include "platform/window/window.h"
#include "rendergraph/Registry.h"
#include "rendergraph/RenderGraph.h"
#include "rendering/types/Barrier.h"
#include "rendering/types/CommandPool.h"
#include "renderview/RenderView.h"
#include "ResourceViewRegistry.h"
#include <glm/gtx/transform.hpp>

#undef CreateSemaphore

void Renderer::Initialize(Window* window, GraphicsBackendType backend)
{
    bool debugLayer   = true;
    m_swapchainWidth  = window->GetWidth();
    m_swapchainHeight = window->GetHeight();

    m_graphicsAdapter = GraphicsAdapter::Create(window->GetRawHandle(), debugLayer, backend);
    m_graphicsAdapter->CreateSwapchain(m_graphicsAdapter->GetGraphicsQueue(), window->GetWidth(), window->GetHeight(), window, PresentMode::IMMEDIATE, &m_swapchain);

    m_graphicsAdapter->CreateSemaphore(0, &m_semaphores[0]);
    m_graphicsAdapter->CreateSemaphore(0, &m_semaphores[1]);
    m_graphicsAdapter->CreateSemaphore(0, &m_semaphores[2]);

    BufferCreateInfo createInfo { 1024 * 1024 * 4, {}, BufferUsageFlags::CONSTANT_BUFFER_BIT };
    m_graphicsAdapter->CreateBuffer(createInfo, MemoryPropertyFlags::HOST_VISIBLE_BIT | MemoryPropertyFlags::HOST_COHERENT_BIT, MemoryPropertyFlags::DEVICE_LOCAL_BIT, false, &m_mappableConstantBuffers[0]);
    m_graphicsAdapter->CreateBuffer(createInfo, MemoryPropertyFlags::HOST_VISIBLE_BIT | MemoryPropertyFlags::HOST_COHERENT_BIT, MemoryPropertyFlags::DEVICE_LOCAL_BIT, false, &m_mappableConstantBuffers[1]);
    m_graphicsAdapter->SetDebugObjectName(ObjectType::BUFFER, m_mappableConstantBuffers[0], "Mappable Constant Buffer 0");
    m_graphicsAdapter->SetDebugObjectName(ObjectType::BUFFER, m_mappableConstantBuffers[1], "Mappable Constant Buffer 1");

    DescriptorSetLayoutBinding binding = { DescriptorType::OFFSET_CONSTANT_BUFFER, 0, 0, 1, ShaderStageFlags::ALL_STAGES };
    m_graphicsAdapter->CreateDescriptorSetLayout(1, &binding, &m_offsetBufferDescriptorSetLayout);
    m_graphicsAdapter->CreateDescriptorSetPool(2, m_offsetBufferDescriptorSetLayout, &m_offsetBufferDescriptorSetPool);
    m_offsetBufferDescriptorSetPool->AllocateDescriptorSets(2, m_offsetBufferDescriptorSets);

    for(size_t i = 0; i < 2; ++i)
    {
	DescriptorSetUpdate update = { 0, 0, 1, DescriptorType::OFFSET_CONSTANT_BUFFER };
	update.BufferInfo1	   = { m_mappableConstantBuffers[i], 0, 1024 /*m_mappableConstantBuffers[i]->getDescription().m_size*/, 0 };
	m_offsetBufferDescriptorSets[i]->Update(1, &update);
    }

    m_viewRegistry = new ResourceViewRegistry(m_graphicsAdapter);
    m_renderGraph  = new RenderGraph(m_graphicsAdapter, m_semaphores, m_semaphoreValues, m_viewRegistry);
    m_renderView   = new RenderView(m_graphicsAdapter, m_viewRegistry, m_offsetBufferDescriptorSetLayout, m_swapchainWidth, m_swapchainHeight);
}

void Renderer::Shutdown()
{
}

void Renderer::Render() noexcept
{
    m_renderGraph->NextFrame();

    RenderView::Data renderViewData = {};
    {
	renderViewData.CameraData = {};
	{
	    renderViewData.CameraData.ViewMatrix       = glm::mat4_cast(glm::inverse(m_rotation)) * glm::translate(-m_position);
	    renderViewData.CameraData.ProjectionMatrix = glm::perspective(1.57079632679f, 1.0f, 0.1f, 300.0f);
	    renderViewData.CameraData.CameraPosition   = m_position;
	}
	renderViewData.OffsetBufferSet = m_offsetBufferDescriptorSets[m_frame & 1];
	renderViewData.ConstantBuffer  = m_mappableConstantBuffers[m_frame & 1];
	renderViewData.FrameWidth      = m_swapchainWidth;
	renderViewData.FrameHeight     = m_swapchainHeight;
    }

    m_renderView->Render(renderViewData, m_renderGraph);

    if(m_swapchainWidth != 0 && m_swapchainHeight != 0)
    {
	auto swapchainIndex		       = m_swapchain->GetCurrentImageIndex();
	ResourceHandle swapchainImageHandle    = m_renderGraph->ImportImage(m_swapchain->GetImage(swapchainIndex), "Swapchain Image");
	ResourceViewHandle swapchainViewHandle = m_renderGraph->CreateImageView(ImageViewDescription::CreateDefault("Swapchain Image View", swapchainImageHandle, m_renderGraph));

	ResourceViewHandle resultImageViewHandle = m_renderView->GetResultImageViewHandle();

	ResourceUsageDescription usageDescs[] = {
	    { resultImageViewHandle, { ResourceState::READ_TRANSFER } },
	    { swapchainViewHandle, { ResourceState::WRITE_TRANSFER } },
	};

	m_renderGraph->AddPass("Copy To Swapchain", QueueType::GRAPHICS, eastl::size(usageDescs), usageDescs, [=](Command* cmdList, const Registry& registry)
	{
	    ImageCopy imageCopy = {};
	    {
		imageCopy.m_srcLayerCount = 1;
		imageCopy.m_dstLayerCount = 1;
		imageCopy.m_extent	  = { m_swapchainWidth, m_swapchainHeight, 1 };
	    }
	    cmdList->CopyImage(registry.GetImage(resultImageViewHandle), registry.GetImage(swapchainViewHandle), 1, &imageCopy);
	});

	ResourceUsageDescription presentTransitionUsageDescription = { swapchainViewHandle, { ResourceState::PRESENT } };
	m_renderGraph->AddPass("Present Transition", QueueType::GRAPHICS, 1, &presentTransitionUsageDescription, [=](Command* command, const Registry&)
	{
	});
    }

    m_viewRegistry->FlushChanges();
    m_renderGraph->Execute();

    if(m_swapchainWidth != 0 && m_swapchainHeight != 0)
    {
	m_swapchain->Present(m_semaphores[0], m_semaphoreValues[0], m_semaphores[0], m_semaphoreValues[0] + 1);
	m_semaphoreValues[0]++;
    }

    m_viewRegistry->SwapSets();

    m_frame++;
}

bool Renderer::ActivateFullscreen(Window* window)
{
    return m_graphicsAdapter->ActivateFullscreen(window);
}