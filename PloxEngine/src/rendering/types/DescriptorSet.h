//
// Created by Ploxie on 2023-05-17.
//

#pragma once

#include "Buffer.h"
#include "Image.h"
#include <cstdint>

class Sampler;
class ImageView;
class BufferView;

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

enum class DescriptorType
{
    SAMPLER		   = 0,
    TEXTURE		   = 1,
    RW_TEXTURE		   = 2,
    TYPED_BUFFER	   = 3,
    RW_TYPED_BUFFER	   = 4,
    CONSTANT_BUFFER	   = 5,
    BYTE_BUFFER		   = 6,
    RW_BYTE_BUFFER	   = 7,
    STRUCTURED_BUFFER	   = 8,
    RW_STRUCTURED_BUFFER   = 9,
    OFFSET_CONSTANT_BUFFER = 10,
    RANGE_SIZE		   = OFFSET_CONSTANT_BUFFER + 1
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

struct BufferViewCreateInfo
{
    Buffer* Buffer;
    Format Format;
    uint64_t Offset;
    uint64_t Range;
};

struct DescriptorBufferInfo
{
    Buffer* Buffer;
    uint64_t Offset;
    uint64_t Range;
    uint32_t StructureByteStride;
};

struct DescriptorSetUpdate
{
    uint32_t DstBinding;
    uint32_t DstArrayElement;
    uint32_t DescriptorCount;
    DescriptorType DescriptorType;
    const Sampler* const* Samplers;
    const ImageView* const* ImageViews;
    const BufferView* const* BufferViews;
    const DescriptorBufferInfo* BufferInfo;
    const Sampler* Sampler;
    const ImageView* ImageView;
    const BufferView* BufferView;
    DescriptorBufferInfo BufferInfo1;
};

class Sampler
{
public:
    virtual ~Sampler()			  = default;
    virtual void* GetNativeHandle() const = 0;
};

class ImageView
{
public:
    virtual ~ImageView()				      = default;
    virtual void* GetNativeHandle() const		      = 0;
    virtual const Image* GetImage() const		      = 0;
    virtual const ImageViewCreateInfo& GetDescription() const = 0;
};

class BufferView
{
public:
    virtual ~BufferView()				       = default;
    virtual void* GetNativeHandle() const		       = 0;
    virtual const Buffer* GetBuffer() const		       = 0;
    virtual const BufferViewCreateInfo& GetDescription() const = 0;
};

class DescriptorSetLayout
{
public:
    virtual ~DescriptorSetLayout()	  = default;
    virtual void* GetNativeHandle() const = 0;
};

class DescriptorSet
{
public:
    virtual ~DescriptorSet()						    = default;
    virtual void* GetNativeHandle() const				    = 0;
    virtual void Update(uint32_t count, const DescriptorSetUpdate* updates) = 0;
};

class DescriptorSetPool
{
public:
    virtual ~DescriptorSetPool()					      = default;
    virtual void* GetNativeHandle() const				      = 0;
    virtual void AllocateDescriptorSets(uint32_t count, DescriptorSet** sets) = 0;
    virtual void Reset()						      = 0;
};
