//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "Extent2D.h"
#include "Format.h"
#include "utility/Utilities.h"
#include <cstdint>

enum class SampleCount
{
    _1	= 0x00000001,
    _2	= 0x00000002,
    _4	= 0x00000004,
    _8	= 0x00000008,
    _16 = 0x00000010,
};

enum class ImageType
{
    _1D = 0,
    _2D = 1,
    _3D = 2,
};

enum class ImageCreateFlags
{
    MUTABLE_FORMAT_BIT	     = 0x00000008,
    CUBE_COMPATIBLE_BIT	     = 0x00000010,
    _2D_ARRAY_COMPATIBLE_BIT = 0x00000020,
};
DEF_ENUM_FLAG_OPERATORS(ImageCreateFlags);

enum class ImageUsageFlags
{
    TRANSFER_SRC_BIT		 = 1u << 0u,
    TRANSFER_DST_BIT		 = 1u << 1u,
    TEXTURE_BIT			 = 1u << 2u,
    RW_TEXTURE_BIT		 = 1u << 3u,
    COLOR_ATTACHMENT_BIT	 = 1u << 4u,
    DEPTH_STENCIL_ATTACHMENT_BIT = 1u << 5u,
    CLEAR_BIT			 = 1u << 6u,
};
DEF_ENUM_FLAG_OPERATORS(ImageUsageFlags);

union ClearColorValue
{
    float F3232[4];
    int32_t Int32[4];
    uint32_t Uint32[4];
};

struct ClearDepthStencilValue
{
    float Depth;
    uint32_t Stencil;
};

union ClearValue
{
    ClearColorValue Color;
    ClearDepthStencilValue DepthStencil;
};

struct ImageCopy
{
    uint32_t m_srcMipLevel;
    uint32_t m_srcBaseLayer;
    uint32_t m_srcLayerCount;
    Offset3D m_srcOffset;
    uint32_t m_dstMipLevel;
    uint32_t m_dstBaseLayer;
    uint32_t m_dstLayerCount;
    Offset3D m_dstOffset;
    Extent3D m_extent;
};

struct ImageCreateInfo
{
    uint32_t Width		 = 1;
    uint32_t Height		 = 1;
    uint32_t Depth		 = 1;
    uint32_t Levels		 = 1;
    uint32_t Layers		 = 1;
    SampleCount Samples		 = SampleCount::_1;
    ImageType ImageType		 = ImageType::_2D;
    Format Format		 = Format::UNDEFINED;
    ImageCreateFlags CreateFlags = static_cast<ImageCreateFlags>(0);
    ImageUsageFlags UsageFlags	 = static_cast<ImageUsageFlags>(0);
    ClearValue OptimizedClearValue;
};

class Image
{
public:
    virtual ~Image()					  = default;
    virtual void* GetNativeHandle() const		  = 0;
    virtual const ImageCreateInfo& GetDescription() const = 0;
};
