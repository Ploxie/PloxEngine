//
// Created by Ploxie on 2023-05-17.
//

#pragma once
#include <cstdint>

class Buffer;
class Sampler;
class ImageView;
class BufferView;

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
