//
// Created by Ploxie on 2023-05-24.
//

#include "RenderView.h"
#include "rendering/GraphicsAdapter.h"
#include "rendering/rendergraph/RenderGraph.h"

RenderView::RenderView(GraphicsAdapter* adapter, ResourceViewRegistry* viewRegistry, DescriptorSetLayout* offsetBufferSetLayout, uint32_t width, uint32_t height) noexcept
    : m_adapter(adapter), m_resourceRegistry(viewRegistry), m_width(width), m_height(height), m_resources({})
{
    CreateImageResources(width, height);

    m_cubePass = new CubePass(adapter, offsetBufferSetLayout);
}

void RenderView::Render(const RenderView::Data& data, RenderGraph* renderGraph) noexcept
{
    const size_t resIndex     = m_frame & 1;
    const size_t prevResIndex = (m_frame + 1) & 1;

    ResourceHandle resultImageHandle = renderGraph->ImportImage(m_resources.ResultImage, "Render View Result", m_resources.ResultImageState);
    m_resultImageViewHandle	     = renderGraph->CreateImageView(ImageViewDescription::CreateDefault("Render View Result", resultImageHandle, renderGraph));

    ResourceHandle depthBufferImageHandle = renderGraph->CreateImage(ImageDescription::Create("Depth Buffer", Format::D32_SFLOAT, ImageUsageFlags::DEPTH_STENCIL_ATTACHMENT_BIT | ImageUsageFlags::TEXTURE_BIT, m_width, m_height));
    m_resultDepthViewHandle		  = renderGraph->CreateImageView(ImageViewDescription::CreateDefault("Depth Buffer", depthBufferImageHandle, renderGraph));

    auto& viewData = m_viewData[resIndex];
    {
	viewData.CameraData	 = data.CameraData;
	viewData.OffsetBufferSet = data.OffsetBufferSet;
	viewData.ConstantBuffer	 = data.ConstantBuffer;
    }

    // Cube Pass
    {
	CubePass::Data cubeData = {};
	{
	    cubeData.CameraData	     = viewData.CameraData;
	    cubeData.ColorAttachment = m_resultImageViewHandle;
	    cubeData.DepthAttachment = m_resultDepthViewHandle;
	    cubeData.OffsetBufferSet = viewData.OffsetBufferSet;
	    cubeData.ConstantBuffer  = viewData.ConstantBuffer;
	    cubeData.FrameWidth	     = data.FrameWidth;
	    cubeData.FrameHeight     = data.FrameHeight;
	}

	m_cubePass->Record(renderGraph, cubeData);
    }

    m_frame++;
}

Image* RenderView::GetResultImage() const noexcept
{
    return m_resources.ResultImage;
}

ImageView* RenderView::GetResultImageView()
{
    return m_resources.ResultImageView;
}

ResourceViewHandle RenderView::GetResultImageViewHandle() const noexcept
{
    return m_resultImageViewHandle;
}

void RenderView::CreateImageResources(uint32_t width, uint32_t height) noexcept
{
    ImageCreateInfo createInfo = {};
    {
	createInfo.Width      = width;
	createInfo.Height     = height;
	createInfo.Format     = Format::B8G8R8A8_UNORM;
	createInfo.UsageFlags = ImageUsageFlags::COLOR_ATTACHMENT_BIT | ImageUsageFlags::TEXTURE_BIT | ImageUsageFlags::RW_TEXTURE_BIT | ImageUsageFlags::TRANSFER_SRC_BIT;
	createInfo.Samples    = SampleCount::_1;
    }

    m_adapter->CreateImage(createInfo, MemoryPropertyFlags::DEVICE_LOCAL_BIT, {}, true, &m_resources.ResultImage);
}
