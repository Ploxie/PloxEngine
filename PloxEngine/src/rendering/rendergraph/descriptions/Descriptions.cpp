//
// Created by Ploxie on 2023-05-23.
//
#include "BufferDescription.h"
#include "ImageDescription.h"
#include "rendering/rendergraph/RenderGraph.h"

ImageDescription ImageDescription::Create(const char* name, enum Format format, ImageUsageFlags usageFlags, uint32_t width, uint32_t height, uint32_t depth, uint32_t layers, uint32_t levels, enum ImageType imageType, ClearValue optimizedClearValue, SampleCount samples)
{
    ImageDescription desc = {};
    {
	desc.Name		 = name;
	desc.Width		 = width;
	desc.Height		 = height;
	desc.Depth		 = depth;
	desc.Layers		 = layers;
	desc.Levels		 = levels;
	desc.Samples		 = samples;
	desc.ImageType		 = imageType;
	desc.Format		 = format;
	desc.UsageFlags		 = usageFlags;
	desc.OptimizedClearValue = optimizedClearValue;
    }

    return desc;
}
ImageViewDescription ImageViewDescription::Create(const char* name, ResourceHandle imageHandle, const ImageSubresourceRange& subresourceRange, ImageViewType viewType, enum Format format, const ComponentMapping& components) noexcept
{
    ImageViewDescription desc = {};
    {
	desc.Name	      = name;
	desc.ImageHandle      = imageHandle;
	desc.SubresourceRange = subresourceRange;
	desc.ViewType	      = viewType;
	desc.Format	      = format;
	desc.Components	      = components;
    }

    return desc;
}

ImageViewDescription ImageViewDescription::CreateDefault(const char* name, ResourceHandle imageHandle, RenderGraph* renderGraph) noexcept
{
    const auto& resDesc = renderGraph->m_resourceDescriptions[imageHandle - 1];

    ImageViewDescription desc = {};
    {
	desc.Name	      = name;
	desc.ImageHandle      = imageHandle;
	desc.SubresourceRange = { 0, resDesc.Levels, 0, resDesc.Layers };
	desc.ViewType	      = static_cast<ImageViewType>(resDesc.ImageType);
	desc.Format	      = resDesc.Format;
	desc.Components	      = {};
    }

    return desc;
}

BufferDescription BufferDescription::Create(const char* name, uint64_t size, BufferUsageFlags usageFlags, bool hostVisible) noexcept
{
    BufferDescription desc = {};
    {
	desc.Name	 = name;
	desc.Size	 = size;
	desc.UsageFlags	 = usageFlags;
	desc.HostVisible = hostVisible;
    }

    return desc;
}

BufferViewDescription BufferViewDescription::Create(const char* name, ResourceHandle handle, uint64_t range, uint64_t offset, uint32_t structureByteStride, enum Format format) noexcept
{
    BufferViewDescription desc;
    {
	desc.Name		 = name;
	desc.BufferHandle	 = handle;
	desc.Offset		 = offset;
	desc.Range		 = range;
	desc.StructureByteStride = structureByteStride;
	desc.Format		 = format;
    }

    return desc;
}
BufferViewDescription BufferViewDescription::CreateDefault(const char* name, ResourceHandle bufferHandle, RenderGraph* renderGraph) noexcept
{
    const auto& resDesc = renderGraph->m_resourceDescriptions[bufferHandle - 1];

    BufferViewDescription desc = {};
    {
	desc.Name		 = name;
	desc.BufferHandle	 = bufferHandle;
	desc.Offset		 = 0;
	desc.Range		 = resDesc.Size;
	desc.StructureByteStride = 0;
	desc.Format		 = Format::UNDEFINED;
    }

    return desc;
}
