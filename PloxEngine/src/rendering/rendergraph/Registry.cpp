//
// Created by Ploxie on 2023-05-23.
//

#include "Registry.h"
#include "core/Assert.h"
#include "RenderGraph.h"

Registry::Registry(RenderGraph* renderGraph) noexcept
    : m_graph(renderGraph)
{
}

uint32_t Registry::GetBindlessHandle(ResourceViewHandle handle, DescriptorType type) const noexcept
{
    const auto& frameResources = m_graph->m_frameResources[m_graph->m_frame % RenderGraph::FRAME_COUNT];
    switch(type)
    {
	case DescriptorType::TEXTURE:
	    return frameResources.ResourceViews[handle - 1].TextureHandle;
	case DescriptorType::RW_TEXTURE:
	    return frameResources.ResourceViews[handle - 1].RWTextureHandle;
	case DescriptorType::TYPED_BUFFER:
	    return frameResources.ResourceViews[handle - 1].TypedBufferHandle;
	case DescriptorType::RW_TYPED_BUFFER:
	    return frameResources.ResourceViews[handle - 1].RWTypedBufferHandle;
	case DescriptorType::BYTE_BUFFER:
	    return frameResources.ResourceViews[handle - 1].ByteBufferHandle;
	case DescriptorType::RW_BYTE_BUFFER:
	    return frameResources.ResourceViews[handle - 1].RWByteBufferHandle;
	case DescriptorType::STRUCTURED_BUFFER:
	    return frameResources.ResourceViews[handle - 1].StructuredBufferHandle;
	case DescriptorType::RW_STRUCTURED_BUFFER:
	    return frameResources.ResourceViews[handle - 1].RWStructuredBufferHandle;
	default:
	    ASSERT(false);
	    break;
    }
    return uint32_t();
}

Image* Registry::GetImage(ResourceHandle handle) const noexcept
{
    const auto& frameResources = m_graph->m_frameResources[m_graph->m_frame % RenderGraph::FRAME_COUNT];
    return frameResources.Resources[(size_t) handle - 1].Image;
}

Image* Registry::GetImage(ResourceViewHandle handle) const noexcept
{
    const auto& frameResources = m_graph->m_frameResources[m_graph->m_frame % RenderGraph::FRAME_COUNT];
    return frameResources.Resources[(size_t) m_graph->m_viewDescriptions[(size_t) handle - 1].ResourceHandle - 1].Image;
}

ImageView* Registry::GetImageView(ResourceViewHandle handle) const noexcept
{
    const auto& frameResources = m_graph->m_frameResources[m_graph->m_frame % RenderGraph::FRAME_COUNT];
    return frameResources.ResourceViews[(size_t) handle - 1].ImageView;
}

Buffer* Registry::GetBuffer(ResourceHandle handle) const noexcept
{
    const auto& frameResources = m_graph->m_frameResources[m_graph->m_frame % RenderGraph::FRAME_COUNT];
    return frameResources.Resources[(size_t) handle - 1].Buffer;
}

Buffer* Registry::GetBuffer(ResourceViewHandle handle) const noexcept
{
    const auto& frameResources = m_graph->m_frameResources[m_graph->m_frame % RenderGraph::FRAME_COUNT];
    return frameResources.Resources[(size_t) m_graph->m_viewDescriptions[(size_t) handle - 1].ResourceHandle - 1].Buffer;
}

void Registry::Map(ResourceViewHandle handle, void** data) const noexcept
{
    const auto& frameResources = m_graph->m_frameResources[m_graph->m_frame % RenderGraph::FRAME_COUNT];

    const auto& viewDesc = m_graph->m_viewDescriptions[(size_t) handle - 1];
    const size_t resIdx	 = (size_t) viewDesc.ResourceHandle - 1;
    const auto& resDesc	 = m_graph->m_resourceDescriptions[resIdx];
    ASSERT(resDesc.HostVisible && !resDesc.External);
    frameResources.Resources[resIdx].Buffer->Map(data);
    *data = ((uint8_t*) *data) + viewDesc.Offset;
}

void Registry::Unmap(ResourceViewHandle handle) const noexcept
{
    const auto& frameResources = m_graph->m_frameResources[m_graph->m_frame % RenderGraph::FRAME_COUNT];

    const auto& viewDesc = m_graph->m_viewDescriptions[(size_t) handle - 1];
    const size_t resIdx	 = (size_t) viewDesc.ResourceHandle - 1;
    const auto& resDesc	 = m_graph->m_resourceDescriptions[resIdx];
    ASSERT(resDesc.HostVisible && !resDesc.External);
    frameResources.Resources[resIdx].Buffer->Unmap();
}
