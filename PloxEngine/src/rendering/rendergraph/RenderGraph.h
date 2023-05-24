//
// Created by Ploxie on 2023-05-23.
//

#pragma once
#include "CommandFramePool.h"
#include "descriptions/ImageDescription.h"
#include "EASTL/bitvector.h"
#include "EASTL/internal/function.h"
#include "EASTL/vector.h"
#include "rendering/rendergraph/descriptions/BufferDescription.h"
#include "rendering/types/Queue.h"
#include "ViewHandles.h"
#include <cstdint>

class GraphicsAdapter;
class Semaphore;
class ResourceViewRegistry;
class BufferView;

struct ResourceStateAndStage
{
    ResourceState ResourceState	 = ResourceState::UNDEFINED;
    PipelineStageFlags StageMask = PipelineStageFlags::TOP_OF_PIPE_BIT;
};

struct ResourceStateData
{
    ResourceStateAndStage StateAndStage;
    Queue* Queue;
};

struct ResourceUsageDescription
{
    ResourceViewHandle ViewHandle;
    ResourceStateAndStage StateAndStage;
    ResourceStateAndStage FinalStateAndStage;
};

class RenderGraph
{
    friend class Registry;
    friend struct ImageViewDescription;
    friend struct BufferViewDescription;

public:
    using RecordFunc = eastl::function<void(Command* command, const Registry& registry)>;

public:
    explicit RenderGraph(GraphicsAdapter* adapter, Semaphore** semaphores, uint64_t* semaphoreValues, ResourceViewRegistry* resourceViewRegistry) noexcept;
    ~RenderGraph() noexcept;

    RenderGraph(const RenderGraph&)		= delete;
    RenderGraph(const RenderGraph&&)		= delete;
    RenderGraph& operator=(const RenderGraph&)	= delete;
    RenderGraph& operator=(const RenderGraph&&) = delete;

    ResourceViewHandle CreateImageView(const ImageViewDescription& viewDesc) noexcept;
    ResourceViewHandle CreateBufferView(const BufferViewDescription& viewDesc) noexcept;
    ResourceHandle CreateImage(const ImageDescription& imageDesc) noexcept;
    ResourceHandle CreateBuffer(const BufferDescription& bufferDesc) noexcept;
    ResourceHandle ImportImage(Image* image, const char* name, ResourceStateData* resourceStateData = nullptr) noexcept;
    ResourceHandle ImportBuffer(Buffer* buffer, const char* name, ResourceStateData* resourceStateData = nullptr) noexcept;

    void NextFrame() noexcept;
    void AddPass(const char* name, QueueType queueType, size_t usageCount, const ResourceUsageDescription* usageDesc, const RecordFunc& recordFunc) noexcept;
    void Execute() noexcept;

private:
    void CreateResources() noexcept;
    void CreateSynchronization() noexcept;
    void RecordAndSubmit() noexcept;

private:
    struct ResourceDescription
    {
	const char* Name		     = "";
	uint32_t UsageFlags		     = 0;
	uint32_t Width			     = 1;
	uint32_t Height			     = 1;
	uint32_t Depth			     = 1;
	uint32_t Layers			     = 1;
	uint32_t Levels			     = 1;
	SampleCount Samples		     = SampleCount::_1;
	ImageType ImageType		     = ImageType::_2D;
	Format Format			     = Format::UNDEFINED;
	uint64_t Offset			     = 0;
	uint64_t Size			     = 0;
	ImageCreateFlags ImageFlags	     = {};
	ClearValue OptimizedClearValue	     = {};
	uint32_t SubresourceCount	     = 1;
	uint32_t SubresourceUsageInfoOffset  = 0;
	ResourceStateData* ExternalStateData = nullptr;
	bool Concurrent			     = false;
	bool External			     = false;
	bool Image			     = false;
	bool HostVisible;
    };

    struct ResourceViewDescription
    {
	const char* Name = "";
	ResourceHandle ResourceHandle;
	ImageViewType ViewType;
	Format Format;
	ComponentMapping Components;
	ImageSubresourceRange SubresourceRange;
	uint64_t Offset;
	uint64_t Range;
	uint32_t StructureByteStride;
	bool Image;
    };

    struct Resource
    {
	Image* Image;
	Buffer* Buffer;
	bool External;
    };

    struct ResourceView
    {
	ImageView* ImageView;
	BufferView* BufferView;
	TextureViewHandle TextureHandle;
	RWTextureViewHandle RWTextureHandle;
	TypedBufferViewHandle TypedBufferHandle;
	RWTypedBufferViewHandle RWTypedBufferHandle;
	ByteBufferViewHandle ByteBufferHandle;
	RWByteBufferViewHandle RWByteBufferHandle;
	StructuredBufferViewHandle StructuredBufferHandle;
	RWStructuredBufferViewHandle RWStructuredBufferHandle;
    };

    struct SubresourceUsage
    {
	uint16_t PassHandle;
	ResourceStateAndStage InitialResourceState;
	ResourceStateAndStage FinalResourceState;
    };

    struct PassData
    {
	RecordFunc RecordFunc;
	const char* Name;
	Queue* Queue;
	uint32_t SignalValue;
	eastl::vector<Barrier> BeforeBarriers;
	eastl::vector<Barrier> AfterBarriers;
    };

    struct Batch
    {
	Queue* Queue;
	uint16_t PassIndexOffset;
	uint16_t PassIndexCount;
	PipelineStageFlags WaitDstStageMasks[3];
	uint64_t WaitValues[3];
	uint64_t SignalValue;
	bool LastBatchOnQueue;
    };

    struct FrameGPUResources
    {
	eastl::vector<Resource> Resources;
	eastl::vector<ResourceView> ResourceViews;
	CommandFramePool CommandFramePool;
	uint64_t FinalWaitValues[3] = {};
    };

    static constexpr size_t FRAME_COUNT = 2;

    uint64_t m_frame = 0;
    GraphicsAdapter* m_adapter;
    Queue* m_queues[3];
    Semaphore* m_semaphores[3];
    uint64_t* m_semaphoreValues[3];
    ResourceViewRegistry* m_resourceViewRegistry;

    eastl::vector<ResourceDescription> m_resourceDescriptions;
    eastl::vector<ResourceViewDescription> m_viewDescriptions;
    eastl::bitvector<> m_culledResources;
    eastl::vector<eastl::vector<SubresourceUsage>> m_subresourceUsages;
    eastl::vector<PassData> m_passData;
    eastl::vector<Batch> m_recordBatches;
    eastl::vector<Barrier> m_externalReleaseBarriers[3];

    FrameGPUResources m_frameResources[FRAME_COUNT];
};