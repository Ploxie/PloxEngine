//
// Created by Ploxie on 2023-05-17.
//

#pragma once
#include "rendering/types/Command.h"
#include "utility/memory/LinearAllocator.h"
#include "vulkan/vulkan.h"

class VulkanGraphicsAdapter;

class ImageSubresourceRange;
class Barrier;

class VulkanCommand : public Command
{
public:
    explicit VulkanCommand(VkCommandBuffer commandBuffer, VulkanGraphicsAdapter* adapter) noexcept;
    void* GetNativeHandle() const override;
    void Begin() override;
    void End() override;
    void BindPipeline(const GraphicsPipeline* pipeline) override;
    void SetViewports(uint32_t firstViewport, uint32_t viewportCount, const Viewport* viewports) override;
    void SetScissors(uint32_t firstScissor, uint32_t scissorCount, const Rect* scissors) override;
    void SetLineWidth(float lineWidth) override;
    void SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override;
    void SetBlendConstants(const float blendConstants[4]) override;
    void SetDepthBounds(float minDepthBounds, float maxDepthBounds) override;
    void SetStencilCompareMask(StencilFaceFlags faceMask, uint32_t compareMask) override;
    void SetStencilWriteMask(StencilFaceFlags faceMask, uint32_t writeMask) override;
    void SetStencilReference(StencilFaceFlags faceMask, uint32_t reference) override;
    void BindDescriptorSets(const GraphicsPipeline* pipeline, uint32_t firstSet, uint32_t count, const DescriptorSet* const* sets, uint32_t offsetCount, uint32_t* offsets) override;
    //void bindDescriptorSets(const ComputePipeline* pipeline, uint32_t firstSet, uint32_t count, const DescriptorSet* const* sets, uint32_t offsetCount, uint32_t* offsets)  = 0;
    //void bindIndexBuffer(const Buffer* buffer, uint64_t offset, IndexType indexType)											    = 0;
    //void bindVertexBuffers(uint32_t firstBinding, uint32_t count, const Buffer* const* buffers, uint64_t* offsets)							    = 0;
    void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
    //void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)					    = 0;
    //void drawIndirect(const Buffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride)									    = 0;
    //void drawIndexedIndirect(const Buffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride)								    = 0;
    //void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)											    = 0;
    //void dispatchIndirect(const Buffer* buffer, uint64_t offset)								 = 0;
    //void copyBuffer(const Buffer* srcBuffer, const Buffer* dstBuffer, uint32_t regionCount, const BufferCopy* regions)		 = 0;
    void CopyImage(const Image* srcImage, const Image* dstImage, uint32_t regionCount, const ImageCopy* regions) override;
    //void copyBufferToImage(const Buffer* srcBuffer, const Image* dstImage, uint32_t regionCount, const BufferImageCopy* regions) = 0;
    //void copyImageToBuffer(const Image* srcImage, const Buffer* dstBuffer, uint32_t regionCount, const BufferImageCopy* regions) = 0;
    //void updateBuffer(const Buffer* dstBuffer, uint64_t dstOffset, uint64_t dataSize, const void* data) = 0;
    //void fillBuffer(const Buffer* dstBuffer, uint64_t dstOffset, uint64_t size, uint32_t data)								  = 0;
    void ClearColorImage(const Image* image, const ClearColorValue* color, uint32_t rangeCount, const ImageSubresourceRange* ranges) override;
    void ClearDepthStencilImage(const Image* image, const ClearDepthStencilValue* depthStencil, uint32_t rangeCount, const ImageSubresourceRange* ranges) override;
    void Barrier(uint32_t count, const class Barrier* barriers) override;
    //void beginQuery(const QueryPool* queryPool, uint32_t query) = 0;
    //void endQuery(const QueryPool* queryPool, uint32_t query) = 0;
    //void resetQueryPool(const QueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount) = 0;
    //void writeTimestamp(PipelineStageFlags pipelineStage, const QueryPool* queryPool, uint32_t query) = 0;
    //void copyQueryPoolResults(const QueryPool* queryPool, uint32_t firstQuery, uint32_t queryCount, const Buffer* dstBuffer, uint64_t dstOffset) = 0;
    void PushConstants(const GraphicsPipeline* pipeline, ShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* values) override;
    //void pushConstants(const ComputePipeline* pipeline, ShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* values)									     = 0;
    void BeginRenderPass(uint32_t colorAttachmentCount, ColorAttachmentDescription* colorAttachments, DepthStencilAttachmentDescription* depthStencilAttachment, const Rect& renderArea, bool rwTextureBufferAccess) override;
    void EndRenderPass() override;
    //void insertDebugLabel(const char *label)																					     = 0;
    //void beginDebugLabel(const char *label)																					     = 0;
    //void endDebugLabel()

private:
    VkCommandBuffer m_commandBuffer;
    VulkanGraphicsAdapter* m_adapter;
    ScopedAllocation m_memoryAllocation;
    LinearAllocator m_allocator;
};
