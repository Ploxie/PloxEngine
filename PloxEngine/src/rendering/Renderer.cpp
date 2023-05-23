//
// Created by Ploxie on 2023-05-09.
//

#include "Renderer.h"
#include "platform/window/window.h"
#include "rendering/types/Barrier.h"
#include "rendering/types/CommandPool.h"
#include "RenderUtilities.h"

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

    DescriptorSetLayoutBinding binding { DescriptorType::OFFSET_CONSTANT_BUFFER, 0, 0, 1, ShaderStageFlags::ALL_STAGES };

    DescriptorSetLayout* offsetBufferDescriptorSetLayout = nullptr;
    DescriptorSetPool* offsetBufferDescriptorSetPool	 = nullptr;

    m_graphicsAdapter->CreateDescriptorSetLayout(1, &binding, &offsetBufferDescriptorSetLayout);
    m_graphicsAdapter->CreateDescriptorSetPool(2, offsetBufferDescriptorSetLayout, &offsetBufferDescriptorSetPool);

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
    DescriptorSetLayoutDeclaration layoutDecl { offsetBufferDescriptorSetLayout, 1, &usedBinding };
    builder.SetPipelineLayoutDescription(1, &layoutDecl, 0, {}, 0, nullptr, 0);

    m_graphicsAdapter->CreateGraphicsPipeline(1, &pipelineCreateInfo, &m_pipeline);
}

void Renderer::Shutdown()
{
}

void Renderer::Render() noexcept
{
    unsigned int imageIndex	= m_swapchain->GetCurrentImageIndex();
    Image* image		= m_swapchain->GetImage(imageIndex);
    ClearColorValue clear	= { 1.0f, 0.0f, 0.0f, 1.0f };
    ImageSubresourceRange range = {
	0,
	1,
	0, 1
    };
    //m_command->Begin();

    ImageView* imageView = nullptr;
    m_graphicsAdapter->CreateImageView(image, &imageView);

    ClearColorValue clearValue = { 0.0f, 0.0f, 0.0f, 0.0f };
    ColorAttachmentDescription attachmentDesc {
	imageView,
	AttachmentLoadOp::CLEAR,
	AttachmentStoreOp::STORE
    };
    DepthStencilAttachmentDescription depthBufferDesc { imageView, AttachmentLoadOp::LOAD, AttachmentStoreOp::STORE, AttachmentLoadOp::DONT_CARE, AttachmentStoreOp::DONT_CARE };

    Rect renderRect { { 0, 0 }, { 800, 600 } };
    m_command->Begin();
    Barrier b0 = RenderUtilities::ImageBarrier(
	image,
	PipelineStageFlags::CLEAR_BIT,
	PipelineStageFlags::TOP_OF_PIPE_BIT,
	ResourceState::UNDEFINED,
	ResourceState::WRITE_COLOR_ATTACHMENT);
    //m_command->Barrier(1, &b0);

    m_command->BeginRenderPass(1, &attachmentDesc, nullptr, renderRect, false);
    {
	//m_command->BindPipeline(m_pipeline);
	//m_command->ClearColorImage(image, &clearValue, 1, &range);

	//m_command->Draw(6, 1, 0, 0);
    }
    m_command->EndRenderPass();

    Barrier b1 = RenderUtilities::ImageBarrier(
	image,
	PipelineStageFlags::TOP_OF_PIPE_BIT,
	PipelineStageFlags::BOTTOM_OF_PIPE_BIT,
	ResourceState::WRITE_COLOR_ATTACHMENT,
	ResourceState::PRESENT);
    //m_command->Barrier(1, &b1);
    m_command->End();

    uint64_t waitValue	  = m_semaphoreValues[imageIndex];
    uint64_t signalValue  = waitValue + 1;
    auto waitDstStageMask = PipelineStageFlags::TOP_OF_PIPE_BIT;

    SubmitInfo submitInfo {};
    submitInfo.WaitSemaphoreCount   = 1;
    submitInfo.WaitSemaphores	    = &m_semaphores[imageIndex];
    submitInfo.WaitValues	    = &waitValue;
    submitInfo.WaitDstStageMask	    = &waitDstStageMask;
    submitInfo.CommandCount	    = 1;
    submitInfo.Commands		    = &m_command;
    submitInfo.SignalSemaphoreCount = 1;
    submitInfo.SignalSemaphores	    = &m_semaphores[imageIndex];
    submitInfo.SignalValues	    = &signalValue;

    m_graphicsAdapter->GetGraphicsQueue()->Submit(1, &submitInfo);

    m_swapchain->Present(m_semaphores[0], m_semaphoreValues[0], m_semaphores[0], m_semaphoreValues[0] + 1, m_command);
    m_semaphoreValues[0]++;

    m_commandPool->Reset();

    m_frame++;
}

bool Renderer::ActivateFullscreen(Window* window)
{
    return m_graphicsAdapter->ActivateFullscreen(window);
}