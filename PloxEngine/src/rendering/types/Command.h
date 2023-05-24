//
// Created by Ploxie on 2023-05-17.
//

#pragma once

#include "DescriptorSet.h"
#include "GraphicsPipeline.h"

class ImageSubresourceRange;
class Barrier;

enum class StencilFaceFlags
{
    FRONT_BIT	   = 1,
    BACK_BIT	   = 2,
    FRONT_AND_BACK = FRONT_BIT | BACK_BIT,
};
DEF_ENUM_FLAG_OPERATORS(StencilFaceFlags);

enum class AttachmentLoadOp
{
    LOAD,
    CLEAR,
    DONT_CARE
};

enum class AttachmentStoreOp
{
    STORE,
    DONT_CARE
};

struct ColorAttachmentDescription
{
    const ImageView* ImageView;
    AttachmentLoadOp LoadOp;
    AttachmentStoreOp StoreOp;
    ClearColorValue ClearValue;
};

struct DepthStencilAttachmentDescription
{
    const ImageView* ImageView;
    AttachmentLoadOp LoadOp;
    AttachmentStoreOp StoreOp;
    AttachmentLoadOp StencilLoadOp;
    AttachmentStoreOp StencilStoreOp;
    ClearDepthStencilValue ClearValue;
    bool ReadOnly;
};

class Command
{
public:
    virtual ~Command()																				    = default;
    virtual void* GetNativeHandle() const																	    = 0;
    virtual void Begin()																			    = 0;
    virtual void End()																				    = 0;
    virtual void BindPipeline(const GraphicsPipeline* pipeline)															    = 0;
    virtual void SetViewports(uint32_t firstViewport, uint32_t viewportCount, const Viewport* viewports)									    = 0;
    virtual void SetScissors(uint32_t firstScissor, uint32_t scissorCount, const Rect* scissors)										    = 0;
    virtual void SetLineWidth(float lineWidth)																	    = 0;
    virtual void SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)									    = 0;
    virtual void SetBlendConstants(const float blendConstants[4])														    = 0;
    virtual void SetDepthBounds(float minDepthBounds, float maxDepthBounds)													    = 0;
    virtual void SetStencilCompareMask(StencilFaceFlags faceMask, uint32_t compareMask)												    = 0;
    virtual void SetStencilWriteMask(StencilFaceFlags faceMask, uint32_t writeMask)												    = 0;
    virtual void SetStencilReference(StencilFaceFlags faceMask, uint32_t reference)												    = 0;
    virtual void BindDescriptorSets(const GraphicsPipeline* pipeline, uint32_t firstSet, uint32_t count, const DescriptorSet* const* sets, uint32_t offsetCount, uint32_t* offsets) = 0;
    //virtual void bindDescriptorSets(const ComputePipeline* pipeline, uint32_t firstSet, uint32_t count, const DescriptorSet* const* sets, uint32_t offsetCount, uint32_t* offsets)  = 0;
    //virtual void bindIndexBuffer(const Buffer* buffer, uint64_t offset, IndexType indexType)											    = 0;
    //virtual void bindVertexBuffers(uint32_t firstBinding, uint32_t count, const Buffer* const* buffers, uint64_t* offsets)							    = 0;
    virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
    //virtual void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)					    = 0;
    //virtual void drawIndirect(const Buffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride)									    = 0;
    //virtual void drawIndexedIndirect(const Buffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride)								    = 0;
    //virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)											    = 0;
    //virtual void dispatchIndirect(const Buffer* buffer, uint64_t offset)								 = 0;
    //virtual void copyBuffer(const Buffer* srcBuffer, const Buffer* dstBuffer, uint32_t regionCount, const BufferCopy* regions)		 = 0;
    virtual void CopyImage(const Image* srcImage, const Image* dstImage, uint32_t regionCount, const ImageCopy* regions) = 0;
    //virtual void copyBufferToImage(const Buffer* srcBuffer, const Image* dstImage, uint32_t regionCount, const BufferImageCopy* regions) = 0;
    //virtual void copyImageToBuffer(const Image* srcImage, const Buffer* dstBuffer, uint32_t regionCount, const BufferImageCopy* regions) = 0;
    //virtual void updateBuffer(const Buffer* dstBuffer, uint64_t dstOffset, uint64_t dataSize, const void* data) = 0;
    //virtual void fillBuffer(const Buffer* dstBuffer, uint64_t dstOffset, uint64_t size, uint32_t data)								  = 0;
    virtual void ClearColorImage(const Image* image, const ClearColorValue* color, uint32_t rangeCount, const ImageSubresourceRange* ranges)			  = 0;
    virtual void ClearDepthStencilImage(const Image* image, const ClearDepthStencilValue* depthStencil, uint32_t rangeCount, const ImageSubresourceRange* ranges) = 0;
    virtual void Barrier(uint32_t count, const Barrier* barriers)												  = 0;
    //virtual void beginQuery(const QueryPool* queryPool, uint32_t query) = 0;
    //virtual void endQuery(const QueryPool* queryPool, uint32_t query) = 0;
    //virtual void resetQueryPool(const QueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount) = 0;
    //virtual void writeTimestamp(PipelineStageFlags pipelineStage, const QueryPool* queryPool, uint32_t query) = 0;
    //virtual void copyQueryPoolResults(const QueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount, const Buffer* dstBuffer, uint64_t dstOffset) = 0;
    virtual void PushConstants(const GraphicsPipeline* pipeline, ShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* values) = 0;
    //virtual void pushConstants(const ComputePipeline* pipeline, ShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* values)									     = 0;
    virtual void BeginRenderPass(uint32_t colorAttachmentCount, ColorAttachmentDescription* colorAttachments, DepthStencilAttachmentDescription* depthStencilAttachment, const Rect& renderArea, bool rwTextureBufferAccess) = 0;
    virtual void EndRenderPass()																							     = 0;
    //virtual void insertDebugLabel(const char *label)																					     = 0;
    //virtual void beginDebugLabel(const char *label)																					     = 0;
    //virtual void endDebugLabel()																							     = 0;
};