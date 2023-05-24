//
// Created by Ploxie on 2023-05-23.
//

#pragma once

#include "rendering/rendergraph/ViewHandles.h"
#include "rendering/types/Barrier.h"
#include "rendering/types/Format.h"
#include "rendering/types/Image.h"
#include "rendering/types/ImageView.h"

class RenderGraph;

struct ImageDescription
{
    static ImageDescription Create(const char* name, Format format, ImageUsageFlags usageFlags, uint32_t width, uint32_t height, uint32_t depth = 1, uint32_t layers = 1, uint32_t levels = 1, ImageType imageType = ImageType::_2D, ClearValue optimizedClearValue = {}, SampleCount samples = SampleCount::_1);

    const char* Name;
    uint32_t Width;
    uint32_t Height;
    uint32_t Depth;
    uint32_t Layers;
    uint32_t Levels;
    SampleCount Samples;
    ImageType ImageType;
    Format Format;
    ImageUsageFlags UsageFlags;
    ClearValue OptimizedClearValue;
};

struct ImageViewDescription
{
    static ImageViewDescription Create(const char* name, ResourceHandle imageHandle, const ImageSubresourceRange& subResourceRange = { 0, 1, 0, 1 }, ImageViewType viewType = ImageViewType::_2D, Format format = Format::UNDEFINED, const ComponentMapping& components = {}) noexcept;
    static ImageViewDescription CreateDefault(const char* name, ResourceHandle imageHandle, RenderGraph* renderGraph) noexcept;

    const char* Name;
    ResourceHandle ImageHandle;
    ImageSubresourceRange SubresourceRange;
    ImageViewType ViewType;
    Format Format;
    ComponentMapping Components;
};