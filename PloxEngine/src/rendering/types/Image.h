//
// Created by Ploxie on 2023-05-11.
//

#pragma once
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
    float m_f3232[4];
    int32_t m_int32[4];
    uint32_t m_uint32[4];
};

struct ClearDepthStencilValue
{
    float m_depth;
    uint32_t m_stencil;
};

union ClearValue
{
    ClearColorValue m_color;
    ClearDepthStencilValue m_depthStencil;
};

struct ImageCreateInfo
{
    uint32_t m_width		   = 1;
    uint32_t m_height		   = 1;
    uint32_t m_depth		   = 1;
    uint32_t m_levels		   = 1;
    uint32_t m_layers		   = 1;
    SampleCount m_samples	   = SampleCount::_1;
    ImageType m_imageType	   = ImageType::_2D;
    Format m_format		   = Format::UNDEFINED;
    ImageCreateFlags m_createFlags = static_cast<ImageCreateFlags>(0);
    ImageUsageFlags m_usageFlags   = static_cast<ImageUsageFlags>(0);
    ClearValue m_optimizedClearValue;
};

class Image
{
public:
    virtual ~Image()					  = default;
    virtual void* GetNativeHandle() const		  = 0;
    virtual const ImageCreateInfo& GetDescription() const = 0;
};
