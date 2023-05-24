//
// Created by Ploxie on 2023-05-09.
//

#include "Renderer.h"
#include "platform/window/window.h"
#include "rendergraph/Registry.h"
#include "rendergraph/RenderGraph.h"
#include "rendering/types/Barrier.h"
#include "rendering/types/CommandPool.h"
#include "RenderUtilities.h"
#include "ResourceViewRegistry.h"

#undef CreateSemaphore

void Renderer::Initialize(Window* window, GraphicsBackendType backend)
{
    bool debugLayer   = true;
    m_graphicsAdapter = GraphicsAdapter::Create(window->GetRawHandle(), debugLayer, backend);
    m_graphicsAdapter->CreateSwapchain(m_graphicsAdapter->GetGraphicsQueue(), window->GetWidth(), window->GetHeight(), window, PresentMode::IMMEDIATE, &m_swapchain);

    m_graphicsAdapter->CreateSemaphore(0, &m_semaphores[0]);
    m_graphicsAdapter->CreateSemaphore(0, &m_semaphores[1]);
    m_graphicsAdapter->CreateSemaphore(0, &m_semaphores[2]);

    m_graphicsAdapter->CreateCommandPool(m_graphicsAdapter->GetGraphicsQueue(), &m_commandPool);
    m_commandPool->Allocate(1, &m_command);

    m_viewRegistry = new ResourceViewRegistry(m_graphicsAdapter);
    m_renderGraph  = new RenderGraph(m_graphicsAdapter, m_semaphores, m_semaphoreValues, m_viewRegistry);

    ////////
    {
	ImageCreateInfo createInfo {};
	createInfo.Width      = 800;
	createInfo.Height     = 600;
	createInfo.Format     = Format::B8G8R8A8_UNORM;
	createInfo.UsageFlags = ImageUsageFlags::COLOR_ATTACHMENT_BIT | ImageUsageFlags::TEXTURE_BIT | ImageUsageFlags::RW_TEXTURE_BIT | ImageUsageFlags::TRANSFER_SRC_BIT;

	m_graphicsAdapter->CreateImage(createInfo, MemoryPropertyFlags::DEVICE_LOCAL_BIT, {}, true, &m_resultImage);
    }

    BufferCreateInfo createInfo { 1024 * 1024 * 4, {}, BufferUsageFlags::CONSTANT_BUFFER_BIT };
    m_graphicsAdapter->CreateBuffer(createInfo, MemoryPropertyFlags::HOST_VISIBLE_BIT | MemoryPropertyFlags::HOST_COHERENT_BIT, MemoryPropertyFlags::DEVICE_LOCAL_BIT, false, &m_mappableConstantBuffers[0]);
    m_graphicsAdapter->CreateBuffer(createInfo, MemoryPropertyFlags::HOST_VISIBLE_BIT | MemoryPropertyFlags::HOST_COHERENT_BIT, MemoryPropertyFlags::DEVICE_LOCAL_BIT, false, &m_mappableConstantBuffers[1]);

    DescriptorSetLayoutBinding binding { DescriptorType::OFFSET_CONSTANT_BUFFER, 0, 0, 1, ShaderStageFlags::ALL_STAGES };
    m_graphicsAdapter->CreateDescriptorSetLayout(1, &binding, &m_offsetBufferDescriptorSetLayout);
    m_graphicsAdapter->CreateDescriptorSetPool(2, m_offsetBufferDescriptorSetLayout, &m_offsetBufferDescriptorSetPool);
    m_offsetBufferDescriptorSetPool->AllocateDescriptorSets(2, m_offsetBufferDescriptorSets);

    for(size_t i = 0; i < 2; ++i)
    {
	DescriptorSetUpdate update { 0, 0, 1, DescriptorType::OFFSET_CONSTANT_BUFFER };
	update.BufferInfo1 = { m_mappableConstantBuffers[i], 0, 1024 /*m_mappableConstantBuffers[i]->getDescription().m_size*/, 0 };
	m_offsetBufferDescriptorSets[i]->Update(1, &update);
    }

    PipelineColorBlendAttachmentState blendState {};
    blendState.BlendEnable	   = true;
    blendState.SrcColorBlendFactor = BlendFactor::SRC_ALPHA;
    blendState.DstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
    blendState.ColorBlendOp	   = BlendOp::ADD;
    blendState.SrcAlphaBlendFactor = BlendFactor::ZERO;
    blendState.DstAlphaBlendFactor = BlendFactor::ONE;
    blendState.AlphaBlendOp	   = BlendOp::ADD;
    blendState.ColorWriteMask	   = ColorComponentFlags::ALL_BITS;

    GraphicsPipelineCreateInfo pipelineCreateInfo {};
    GraphicsPipelineBuilder builder(pipelineCreateInfo);
    builder.SetVertexShader("assets/shaders/grid_vs");
    builder.SetFragmentShader("assets/shaders/grid_ps");
    builder.SetColorBlendAttachment(blendState);
    builder.SetDepthTest(true, false, CompareOp::GREATER_OR_EQUAL);
    builder.SetDynamicState(DynamicStateFlags::VIEWPORT_BIT | DynamicStateFlags::SCISSOR_BIT);
    builder.SetDepthStencilAttachmentFormat(Format::D32_SFLOAT);
    builder.SetColorAttachmentFormat(Format::B8G8R8A8_UNORM);

    DescriptorSetLayoutBinding usedBinding { DescriptorType::OFFSET_CONSTANT_BUFFER, 0, 0, 1, ShaderStageFlags::ALL_STAGES };
    DescriptorSetLayoutDeclaration layoutDecl { m_offsetBufferDescriptorSetLayout, 0, &usedBinding };
    builder.SetPipelineLayoutDescription(1, &layoutDecl, 0, {}, 0, nullptr, 0);

    m_graphicsAdapter->CreateGraphicsPipeline(1, &pipelineCreateInfo, &m_pipeline);
}

void Renderer::Shutdown()
{
}

void Renderer::Render() noexcept
{
    m_renderGraph->NextFrame();

    //ResourceHandle resultImageHandle	     = m_renderGraph->ImportImage(m_resultImage, "Render View Result", m_resultImageState);
    //ResourceViewHandle resultImageViewHandle = m_renderGraph->CreateImageView(ImageViewDescription::CreateDefault("Render View Result", resultImageHandle, m_renderGraph));

    auto swapchainIndex			   = m_swapchain->GetCurrentImageIndex();
    ResourceHandle swapchainImageHandle	   = m_renderGraph->ImportImage(m_swapchain->GetImage(swapchainIndex), "Swapchain Image");
    ResourceViewHandle swapchainViewHandle = m_renderGraph->CreateImageView(ImageViewDescription::CreateDefault("Swapchain Image View", swapchainImageHandle, m_renderGraph));

    Rect asd;

    ResourceUsageDescription desc = { swapchainViewHandle, ResourceState::WRITE_COLOR_ATTACHMENT };
    m_renderGraph->AddPass("Test Pass", QueueType::GRAPHICS, 1, &desc, [=](Command* command, const Registry& registry)
			   {
			       ColorAttachmentDescription attachmentDesc { registry.GetImageView(swapchainViewHandle), AttachmentLoadOp::CLEAR, AttachmentStoreOp::STORE, { 1.0f, 0.0f, 0.0f, 1.0f } };

			       command->BeginRenderPass(1, &attachmentDesc, nullptr, { { 0, 0 }, { 800, 600 } }, false);
			       {
				   command->BindPipeline(m_pipeline);

				   Viewport viewport { 0.0f, 0.0f, (float) 1, (float) 1, 0.0f, 1.0f };
				   command->SetViewports(0, 1, &viewport);
				   Rect scissor { { 0, 0 }, { 1, 1 } };
				   command->SetScissors(0, 1, &scissor);

				   //command->BindDescriptorSets(m_pipeline, 0, 1, &m_offsetBufferSet, 1, &allocOffset);

				   command->Draw(6, 1, 0, 0);
			       }
			       command->EndRenderPass(); });

    if(m_swapchainWidth != 0 && m_swapchainHeight != 0)
    {
	/*{
	    ResourceUsageDescription usageDescs[] = {
		{ resultImageViewHandle, { ResourceState::READ_TRANSFER } },
		{ swapchainViewHandle, { ResourceState::WRITE_TRANSFER } },
	    };

	    m_renderGraph->AddPass("Copy To Swapchain", QueueType::GRAPHICS, eastl::size(usageDescs), usageDescs, [=](Command* cmdList, const Registry& registry)
				   {
				       ImageCopy imageCopy{};
				       imageCopy.m_srcLayerCount = 1;
				       imageCopy.m_dstLayerCount = 1;
				       imageCopy.m_extent = { m_swapchainWidth, m_swapchainHeight, 1 };
				       cmdList->CopyImage(registry.GetImage(resultImageViewHandle), registry.GetImage(swapchainViewHandle), 1, &imageCopy); });
	}*/

	ResourceUsageDescription presentTransitionUsageDescription = { swapchainViewHandle, { ResourceState::PRESENT } };
	m_renderGraph->AddPass("Present Transition", QueueType::GRAPHICS, 1, &presentTransitionUsageDescription, [=](Command* command, const Registry&) {});
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