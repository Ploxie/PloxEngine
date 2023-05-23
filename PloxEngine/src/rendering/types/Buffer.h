//
// Created by Ploxie on 2023-05-17.
//

#pragma once
#include "utility/Enum.h"
#include <cstdint>

enum class BufferUsageFlags
{
    TRANSFER_SRC_BIT	      = 1u << 0u,
    TRANSFER_DST_BIT	      = 1u << 1u,
    TYPED_BUFFER_BIT	      = 1u << 2u,
    RW_TYPED_BUFFER_BIT	      = 1u << 3u,
    CONSTANT_BUFFER_BIT	      = 1u << 4u,
    BYTE_BUFFER_BIT	      = 1u << 5u,
    RW_BYTE_BUFFER_BIT	      = 1u << 6u,
    STRUCTURED_BUFFER_BIT     = 1u << 7u,
    RW_STRUCTURED_BUFFER_BIT  = 1u << 8u,
    INDEX_BUFFER_BIT	      = 1u << 9u,
    VERTEX_BUFFER_BIT	      = 1u << 10u,
    INDIRECT_BUFFER_BIT	      = 1u << 11u,
    CLEAR_BIT		      = 1u << 12u,
    SHADER_DEVICE_ADDRESS_BIT = 1u << 13u
};
DEF_ENUM_FLAG_OPERATORS(BufferUsageFlags)

enum class BufferCreateFlags
{

};
DEF_ENUM_FLAG_OPERATORS(BufferCreateFlags)

struct BufferCreateInfo
{
    uint64_t Size		  = 1;
    BufferCreateFlags CreateFlags = static_cast<BufferCreateFlags>(0);
    BufferUsageFlags UsageFlags	  = static_cast<BufferUsageFlags>(0);
};

struct MemoryRange
{
    uint64_t m_offset;
    uint64_t m_size;
};

class Buffer
{
public:
    virtual ~Buffer()						       = default;
    virtual void* GetNativeHandle() const			       = 0;
    virtual const BufferCreateInfo& GetDescription() const	       = 0;
    virtual void Map(void** data)				       = 0;
    virtual void Unmap()					       = 0;
    virtual void Invalidate(uint32_t count, const MemoryRange* ranges) = 0;
    virtual void Flush(uint32_t count, const MemoryRange* ranges)      = 0;
};
