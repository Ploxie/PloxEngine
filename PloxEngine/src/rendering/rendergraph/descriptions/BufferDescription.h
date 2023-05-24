//
// Created by Ploxie on 2023-05-23.
//

#pragma once
#include "rendering/rendergraph/ViewHandles.h"
#include "rendering/types/Buffer.h"
#include "rendering/types/Format.h"

class RenderGraph;

struct BufferDescription
{
    static BufferDescription Create(const char* name, uint64_t size, BufferUsageFlags usageFlags, bool hostVisible = false) noexcept;

    const char* Name;
    uint64_t Size;
    BufferUsageFlags UsageFlags;
    bool HostVisible;
};

struct BufferViewDescription
{
    static BufferViewDescription Create(const char* name, ResourceHandle bufferHandle, uint64_t range, uint64_t offset = 0, uint32_t structureByteStride = 0, Format format = Format::UNDEFINED) noexcept;
    static BufferViewDescription CreateDefault(const char* name, ResourceHandle bufferHandle, RenderGraph* renderGraph) noexcept;

    const char* Name;
    ResourceHandle BufferHandle;
    uint64_t Offset;
    uint64_t Range;
    uint32_t StructureByteStride;
    Format Format;
};