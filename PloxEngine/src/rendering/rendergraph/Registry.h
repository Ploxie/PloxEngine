//
// Created by Ploxie on 2023-05-23.
//

#pragma once
#include "rendering/types/DescriptorSet.h"
#include "ViewHandles.h"

class RenderGraph;
class Image;

class Registry
{
public:
    explicit Registry(RenderGraph* renderGraph) noexcept;

    uint32_t GetBindlessHandle(ResourceViewHandle handle, DescriptorType type) const noexcept;

    Image* GetImage(ResourceHandle handle) const noexcept;
    Image* GetImage(ResourceViewHandle handle) const noexcept;
    ImageView* GetImageView(ResourceViewHandle handle) const noexcept;
    Buffer* GetBuffer(ResourceHandle handle) const noexcept;
    Buffer* GetBuffer(ResourceViewHandle handle) const noexcept;

    void Map(ResourceViewHandle handle, void** data) const noexcept;
    void Unmap(ResourceViewHandle handle) const noexcept;

private:
    RenderGraph* m_graph;
};