//
// Created by Ploxie on 2023-05-23.
//

#pragma once

#include "Format.h"
#include <cstdint>

class Image;

enum class ImageViewType
{
    _1D	       = 0,
    _2D	       = 1,
    _3D	       = 2,
    CUBE       = 3,
    _1D_ARRAY  = 4,
    _2D_ARRAY  = 5,
    CUBE_ARRAY = 6,
};

enum class ComponentSwizzle
{
    IDENTITY = 0,
    ZERO     = 1,
    ONE	     = 2,
    R	     = 3,
    G	     = 4,
    B	     = 5,
    A	     = 6,
};

struct ComponentMapping
{
    ComponentSwizzle m_r;
    ComponentSwizzle m_g;
    ComponentSwizzle m_b;
    ComponentSwizzle m_a;
};

struct ImageViewCreateInfo
{
    Image* Image;
    ImageViewType ViewType	= ImageViewType::_2D;
    Format Format		= Format::UNDEFINED;
    ComponentMapping Components = {};
    uint32_t BaseMipLevel	= 0;
    uint32_t LevelCount		= 1;
    uint32_t BaseArrayLayer	= 0;
    uint32_t LayerCount		= 1;
};

class ImageView
{
public:
    virtual ~ImageView()				      = default;
    virtual void* GetNativeHandle() const		      = 0;
    virtual const Image* GetImage() const		      = 0;
    virtual const ImageViewCreateInfo& GetDescription() const = 0;
};
