//
// Created by Ploxie on 2023-05-22.
//

#pragma once

#include "utility/Enum.h"
class Image;
class Buffer;
class Queue;

enum class PipelineStageFlags
{
    TOP_OF_PIPE_BIT		= 0x00000001,
    DRAW_INDIRECT_BIT		= 0x00000002,
    VERTEX_INPUT_BIT		= 0x00000004,
    VERTEX_SHADER_BIT		= 0x00000008,
    HULL_SHADER_BIT		= 0x00000010,
    DOMAIN_SHADER_BIT		= 0x00000020,
    GEOMETRY_SHADER_BIT		= 0x00000040,
    PIXEL_SHADER_BIT		= 0x00000080,
    EARLY_FRAGMENT_TESTS_BIT	= 0x00000100,
    LATE_FRAGMENT_TESTS_BIT	= 0x00000200,
    COLOR_ATTACHMENT_OUTPUT_BIT = 0x00000400,
    COMPUTE_SHADER_BIT		= 0x00000800,
    TRANSFER_BIT		= 0x00001000,
    BOTTOM_OF_PIPE_BIT		= 0x00002000,
    HOST_BIT			= 0x00004000,
    CLEAR_BIT			= 0x00050000,
};
DEF_ENUM_FLAG_OPERATORS(PipelineStageFlags);

enum class ResourceState
{
    UNDEFINED		   = 0,
    READ_RESOURCE	   = 1 << 0,
    READ_DEPTH_STENCIL	   = 1 << 1,
    READ_CONSTANT_BUFFER   = 1 << 2,
    READ_VERTEX_BUFFER	   = 1 << 3,
    READ_INDEX_BUFFER	   = 1 << 4,
    READ_INDIRECT_BUFFER   = 1 << 5,
    READ_TRANSFER	   = 1 << 6,
    WRITE_DEPTH_STENCIL	   = 1 << 7,
    WRITE_COLOR_ATTACHMENT = 1 << 8,
    WRITE_TRANSFER	   = 1 << 9,
    CLEAR_RESOURCE	   = 1 << 10,
    RW_RESOURCE		   = 1 << 11,
    RW_RESOURCE_READ_ONLY  = 1 << 12,
    RW_RESOURCE_WRITE_ONLY = 1 << 13,
    PRESENT		   = 1 << 14,
};
DEF_ENUM_FLAG_OPERATORS(ResourceState);

enum class BarrierFlags
{
    QUEUE_OWNERSHIP_RELEASE    = 1u << 0u,
    QUEUE_OWNERSHIP_AQUIRE     = 1u << 1u,
    FIRST_ACCESS_IN_SUBMISSION = 1u << 2u,
    BARRIER_BEGIN	       = 1u << 3u,
    BARRIER_END		       = 1u << 4u
};
DEF_ENUM_FLAG_OPERATORS(BarrierFlags);

struct ImageSubresourceRange
{
    uint32_t BaseMipLevel   = 0;
    uint32_t LevelCount	    = 1;
    uint32_t BaseArrayLayer = 0;
    uint32_t LayerCount	    = 1;
};

struct Barrier
{
    const Image* m_image;
    const Buffer* m_buffer;
    PipelineStageFlags m_stagesBefore;
    PipelineStageFlags m_stagesAfter;
    ResourceState m_stateBefore;
    ResourceState m_stateAfter;
    Queue* m_srcQueue;
    Queue* m_dstQueue;
    ImageSubresourceRange m_imageSubresourceRange;
    BarrierFlags m_flags;
};