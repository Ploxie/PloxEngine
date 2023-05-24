//
// Created by Ploxie on 2023-05-23.
//
#include "RenderGraph.h"
#include "Registry.h"
#include "rendering/GraphicsAdapter.h"
#include "rendering/RenderUtilities.h"
#include "rendering/ResourceViewRegistry.h"
#include "rendering/types/BufferView.h"
#include "rendering/types/Command.h"
#include "rendering/types/ImageView.h"

RenderGraph::RenderGraph(GraphicsAdapter* adapter, Semaphore** semaphores, uint64_t* semaphoreValues, ResourceViewRegistry* resourceViewRegistry) noexcept
    : m_adapter(adapter), m_resourceViewRegistry(resourceViewRegistry)
{
    m_queues[0] = m_adapter->GetGraphicsQueue();
    m_queues[1] = m_adapter->GetComputeQueue();
    m_queues[2] = m_adapter->GetTransferQueue();

    for(size_t i = 0; i < 3; i++)
    {
	m_semaphores[i]	     = semaphores[i];
	m_semaphoreValues[i] = semaphoreValues + i;
    }

    for(size_t i = 0; i < FRAME_COUNT; i++)
    {
	m_frameResources[i].CommandFramePool.Initialize(m_adapter);
    }
}

RenderGraph::~RenderGraph() noexcept
{
    for(size_t i = 0; i < FRAME_COUNT; i++)
    {
	NextFrame();
    }
}

ResourceViewHandle RenderGraph::CreateImageView(const ImageViewDescription& viewDesc) noexcept
{
    auto& resDesc = m_resourceDescriptions[viewDesc.ImageHandle - 1];
    ASSERT(resDesc.Image);

    ResourceViewDescription desc = {};
    {
	desc.Name	      = viewDesc.Name;
	desc.ResourceHandle   = viewDesc.ImageHandle;
	desc.ViewType	      = viewDesc.ViewType;
	desc.Format	      = viewDesc.Format == Format::UNDEFINED ? resDesc.Format : viewDesc.Format;
	desc.Components	      = viewDesc.Components;
	desc.SubresourceRange = viewDesc.SubresourceRange;
	desc.Image	      = true;
    }

    m_viewDescriptions.push_back(desc);

    if(viewDesc.Format != resDesc.Format)
    {
	resDesc.ImageFlags |= ImageCreateFlags::MUTABLE_FORMAT_BIT;
    }
    if(viewDesc.ViewType == ImageViewType::CUBE || viewDesc.ViewType == ImageViewType::CUBE_ARRAY)
    {
	resDesc.ImageFlags |= ImageCreateFlags::CUBE_COMPATIBLE_BIT;
    }
    if(resDesc.ImageType == ImageType::_3D && viewDesc.ViewType == ImageViewType::_2D_ARRAY)
    {
	resDesc.ImageFlags |= ImageCreateFlags::_2D_ARRAY_COMPATIBLE_BIT;
    }

    return ResourceViewHandle(m_viewDescriptions.size());
}

ResourceViewHandle RenderGraph::CreateBufferView(const BufferViewDescription& viewDesc) noexcept
{
    ASSERT(!m_resourceDescriptions[viewDesc.BufferHandle - 1].Image);

    ResourceViewDescription desc = {};
    {
	desc.Name		 = viewDesc.Name;
	desc.ResourceHandle	 = viewDesc.BufferHandle;
	desc.Format		 = viewDesc.Format;
	desc.Offset		 = viewDesc.Offset;
	desc.Range		 = viewDesc.Range;
	desc.StructureByteStride = viewDesc.StructureByteStride;
    }

    m_viewDescriptions.push_back(desc);

    return ResourceViewHandle(m_viewDescriptions.size());
}

ResourceHandle RenderGraph::CreateImage(const ImageDescription& imageDesc) noexcept
{
    ResourceDescription desc = {};
    {
	desc.Name			= imageDesc.Name;
	desc.UsageFlags			= static_cast<uint32_t>(imageDesc.UsageFlags);
	desc.Width			= imageDesc.Width;
	desc.Height			= imageDesc.Height;
	desc.Depth			= imageDesc.Depth;
	desc.Layers			= imageDesc.Layers;
	desc.Levels			= imageDesc.Levels;
	desc.Samples			= imageDesc.Samples;
	desc.ImageType			= imageDesc.ImageType;
	desc.Format			= imageDesc.Format;
	desc.OptimizedClearValue	= imageDesc.OptimizedClearValue;
	desc.SubresourceCount		= desc.Layers * desc.Levels;
	desc.SubresourceUsageInfoOffset = static_cast<uint32_t>(m_subresourceUsages.size());
	desc.Concurrent			= false;
	desc.Image			= true;
    }

    ASSERT(desc.Width && desc.Height && desc.Layers && desc.Levels);

    m_resourceDescriptions.push_back(desc);
    for(size_t i = 0; i < desc.SubresourceCount; ++i)
    {
	m_subresourceUsages.push_back({});
    }

    return ResourceHandle(m_resourceDescriptions.size());
}

ResourceHandle RenderGraph::CreateBuffer(const BufferDescription& bufferDesc) noexcept
{
    ResourceDescription desc = {};
    {
	desc.Name			= bufferDesc.Name;
	desc.UsageFlags			= static_cast<uint32_t>(bufferDesc.UsageFlags);
	desc.Offset			= 0;
	desc.Size			= bufferDesc.Size;
	desc.SubresourceCount		= 1;
	desc.SubresourceUsageInfoOffset = static_cast<uint32_t>(m_subresourceUsages.size());
	desc.Concurrent			= true;
	desc.HostVisible		= bufferDesc.HostVisible;
    }

    ASSERT(desc.Size);

    m_resourceDescriptions.push_back(desc);
    m_subresourceUsages.push_back({});

    return ResourceHandle(m_resourceDescriptions.size());
}

ResourceHandle RenderGraph::ImportImage(Image* image, const char* name, ResourceStateData* resourceStateData) noexcept
{
    const auto& imageDesc = image->GetDescription();

    ResourceDescription desc = {};
    {
	desc.Name			= name;
	desc.UsageFlags			= static_cast<uint32_t>(imageDesc.UsageFlags);
	desc.Width			= imageDesc.Width;
	desc.Height			= imageDesc.Height;
	desc.Depth			= imageDesc.Depth;
	desc.Layers			= imageDesc.Layers;
	desc.Levels			= imageDesc.Levels;
	desc.Samples			= imageDesc.Samples;
	desc.ImageType			= imageDesc.ImageType;
	desc.Format			= imageDesc.Format;
	desc.OptimizedClearValue	= imageDesc.OptimizedClearValue;
	desc.SubresourceCount		= desc.Layers * desc.Levels;
	desc.SubresourceUsageInfoOffset = static_cast<uint32_t>(m_subresourceUsages.size());
	desc.ExternalStateData		= resourceStateData;
	desc.Concurrent			= false;
	desc.External			= true;
	desc.Image			= true;
    }

    ASSERT(desc.Width && desc.Height && desc.Layers && desc.Levels);

    m_resourceDescriptions.push_back(desc);
    for(size_t i = 0; i < desc.SubresourceCount; ++i)
    {
	m_subresourceUsages.push_back({});
    }

    auto& frameResources = m_frameResources[m_frame % FRAME_COUNT];
    frameResources.Resources.resize(m_resourceDescriptions.size());
    frameResources.Resources.back().Image    = image;
    frameResources.Resources.back().External = true;

    m_adapter->SetDebugObjectName(ObjectType::IMAGE, image, name);

    return ResourceHandle(m_resourceDescriptions.size());
}

ResourceHandle RenderGraph::ImportBuffer(Buffer* buffer, const char* name, ResourceStateData* resourceStateData) noexcept
{
    const auto& bufferDesc = buffer->GetDescription();

    ResourceDescription resDesc = {};
    {
	resDesc.Name			   = name;
	resDesc.UsageFlags		   = static_cast<uint32_t>(bufferDesc.UsageFlags);
	resDesc.Offset			   = 0; // TODO
	resDesc.Size			   = bufferDesc.Size;
	resDesc.SubresourceCount	   = 1;
	resDesc.SubresourceUsageInfoOffset = static_cast<uint32_t>(m_subresourceUsages.size());
	resDesc.ExternalStateData	   = resourceStateData;
	resDesc.Concurrent		   = true;
	resDesc.External		   = true;
    }

    ASSERT(resDesc.Size);

    m_resourceDescriptions.push_back(resDesc);
    m_subresourceUsages.push_back({});

    auto& frameResources = m_frameResources[m_frame % FRAME_COUNT];
    frameResources.Resources.resize(m_resourceDescriptions.size());
    frameResources.Resources.back().Buffer   = buffer;
    frameResources.Resources.back().External = true;

    m_adapter->SetDebugObjectName(ObjectType::BUFFER, buffer, name);

    return ResourceHandle(m_resourceDescriptions.size());
}

void RenderGraph::NextFrame() noexcept
{
    ++m_frame;

    // reset old frame resources
    {
	auto& frameResources = m_frameResources[m_frame % FRAME_COUNT];

	// wait on semaphores for queue completion
	for(size_t i = 0; i < 3; ++i)
	{
	    m_semaphores[i]->Wait(frameResources.FinalWaitValues[i]);
	}

	// destroy internal resources
	for(auto& res: frameResources.Resources)
	{
	    if(res.External)
	    {
		continue;
	    }
	    if(res.Image)
	    {
		m_adapter->DestroyImage(res.Image);
	    }
	    else if(res.Buffer)
	    {
		m_adapter->DestroyBuffer(res.Buffer);
	    }
	}
	frameResources.Resources.clear();

	// destroy views
	for(auto& view: frameResources.ResourceViews)
	{
	    if(view.ImageView)
	    {
		m_adapter->DestroyImageView(view.ImageView);
	    }
	    else if(view.BufferView)
	    {
		m_adapter->DestroyBufferView(view.BufferView);
	    }
	}
	frameResources.ResourceViews.clear();

	// reset command lists
	frameResources.CommandFramePool.Reset();
    }

    // clear old vectors
    m_resourceDescriptions.clear();
    m_viewDescriptions.clear();
    m_culledResources.clear();
    m_subresourceUsages.clear();
    m_passData.clear();
    m_recordBatches.clear();
    m_externalReleaseBarriers[0].clear();
    m_externalReleaseBarriers[1].clear();
    m_externalReleaseBarriers[2].clear();
}

void RenderGraph::AddPass(const char* name, QueueType queueType, size_t usageCount, const ResourceUsageDescription* usageDescs, const RenderGraph::RecordFunc& recordFunc) noexcept
{
#ifdef _DEBUG
    for(uint32_t i = 0; i < usageCount; ++i)
    {
	// catches most false api usages
	ASSERT(usageDescs[i].ViewHandle);
	// not a very good test, but if the handle has a higher value than the number of views, something must be off
	ASSERT(size_t(usageDescs[i].ViewHandle) <= m_viewDescriptions.size());
    }
#endif // _DEBUG

    PassData passData {};
    passData.RecordFunc = recordFunc;
    passData.Name	= name;
    passData.Queue	= m_queues[static_cast<size_t>(queueType)];

    const uint16_t passIndex = static_cast<uint16_t>(m_passData.size());
    m_passData.push_back(passData);

    for(size_t i = 0; i < usageCount; ++i)
    {
	const auto& usage = usageDescs[i];
	ASSERT(usage.ViewHandle != 0);
	const auto& viewDesc = m_viewDescriptions[usage.ViewHandle - 1];
	const SubresourceUsage resUsage { passIndex, usage.StateAndStage, usage.FinalStateAndStage.ResourceState == ResourceState::UNDEFINED ? usage.StateAndStage : usage.FinalStateAndStage };

	const size_t resIndex = (size_t) viewDesc.ResourceHandle - 1;
	const auto& resDesc   = m_resourceDescriptions[resIndex];

	if(resDesc.Image)
	{
	    const uint32_t baseLayer  = viewDesc.SubresourceRange.BaseArrayLayer;
	    const uint32_t layerCount = viewDesc.SubresourceRange.LayerCount;
	    const uint32_t baseLevel  = viewDesc.SubresourceRange.BaseMipLevel;
	    const uint32_t levelCount = viewDesc.SubresourceRange.LevelCount;

	    for(uint32_t layer = 0; layer < layerCount; ++layer)
	    {
		for(uint32_t level = 0; level < levelCount; ++level)
		{
		    const uint32_t index = (layer + baseLayer) * resDesc.Levels + (level + baseLevel) + resDesc.SubresourceUsageInfoOffset;
		    m_subresourceUsages[index].push_back(resUsage);
		}
	    }
	}
	else
	{
	    m_subresourceUsages[resDesc.SubresourceUsageInfoOffset].push_back(resUsage);
	}
    }
}

void RenderGraph::Execute() noexcept
{
    CreateResources();
    CreateSynchronization();
    m_resourceViewRegistry->FlushChanges();
    RecordAndSubmit();
}

void RenderGraph::CreateResources() noexcept
{
    auto& frameResources = m_frameResources[m_frame % FRAME_COUNT];
    frameResources.Resources.resize(m_resourceDescriptions.size());
    m_culledResources.resize(m_resourceDescriptions.size());
    frameResources.ResourceViews.resize(m_viewDescriptions.size());

    // create resources
    const size_t resourceCount = m_resourceDescriptions.size();
    for(size_t resourceIdx = 0; resourceIdx < resourceCount; ++resourceIdx)
    {
	const auto& resDesc = m_resourceDescriptions[resourceIdx];

	if(resDesc.External)
	{
	    continue;
	}

	// check if resource is used at all and also find usage flags
	bool isReferenced   = false;
	uint32_t usageFlags = resDesc.UsageFlags;

	const size_t subresourceCount = resDesc.SubresourceCount;
	for(size_t subresourceIdx = 0; subresourceIdx < subresourceCount; ++subresourceIdx)
	{
	    const size_t subresourceUsageIdx = subresourceIdx + resDesc.SubresourceUsageInfoOffset;

	    // check if subresource is used at all
	    isReferenced = isReferenced || !m_subresourceUsages[subresourceUsageIdx].empty();

	    // get usage flags
	    for(const auto& usage: m_subresourceUsages[subresourceUsageIdx])
	    {
		usageFlags |= RenderUtilities::GetUsageFlags(usage.InitialResourceState.ResourceState, resDesc.Image);
		usageFlags |= RenderUtilities::GetUsageFlags(usage.FinalResourceState.ResourceState, resDesc.Image);
	    }
	}

	// resource has no references -> no need to create an actual resource
	if(!isReferenced)
	{
	    m_culledResources[resourceIdx] = true;
	    continue;
	}

	// is resource image or buffer?
	if(resDesc.Image)
	{
	    // create image
	    ImageCreateInfo imageCreateInfo {};
	    imageCreateInfo.Width		= resDesc.Width;
	    imageCreateInfo.Height		= resDesc.Height;
	    imageCreateInfo.Depth		= resDesc.Depth;
	    imageCreateInfo.Levels		= resDesc.Levels;
	    imageCreateInfo.Layers		= resDesc.Layers;
	    imageCreateInfo.Samples		= resDesc.Samples;
	    imageCreateInfo.ImageType		= resDesc.ImageType;
	    imageCreateInfo.Format		= resDesc.Format;
	    imageCreateInfo.CreateFlags		= resDesc.ImageFlags;
	    imageCreateInfo.UsageFlags		= static_cast<ImageUsageFlags>(usageFlags);
	    imageCreateInfo.OptimizedClearValue = resDesc.OptimizedClearValue;

	    m_adapter->CreateImage(imageCreateInfo, MemoryPropertyFlags::DEVICE_LOCAL_BIT, {}, false, &frameResources.Resources[resourceIdx].Image);
	    m_adapter->SetDebugObjectName(ObjectType::IMAGE, frameResources.Resources[resourceIdx].Image, resDesc.Name);
	}
	else
	{
	    // create buffer
	    BufferCreateInfo bufferCreateInfo {};
	    bufferCreateInfo.Size	 = resDesc.Size;
	    bufferCreateInfo.CreateFlags = {};
	    bufferCreateInfo.UsageFlags	 = static_cast<BufferUsageFlags>(usageFlags);

	    auto requiredFlags	= resDesc.HostVisible ? (MemoryPropertyFlags::HOST_VISIBLE_BIT | MemoryPropertyFlags::HOST_COHERENT_BIT) : MemoryPropertyFlags::DEVICE_LOCAL_BIT;
	    auto preferredFlags = resDesc.HostVisible ? MemoryPropertyFlags::DEVICE_LOCAL_BIT : MemoryPropertyFlags {};

	    m_adapter->CreateBuffer(bufferCreateInfo, requiredFlags, preferredFlags, false, &frameResources.Resources[resourceIdx].Buffer);
	    m_adapter->SetDebugObjectName(ObjectType::BUFFER, frameResources.Resources[resourceIdx].Buffer, resDesc.Name);
	}
    }

    // create views
    const size_t viewCount = m_viewDescriptions.size();
    for(uint32_t viewIndex = 0; viewIndex < m_viewDescriptions.size(); ++viewIndex)
    {
	const auto& viewDesc = m_viewDescriptions[viewIndex];

	if(m_culledResources[viewDesc.ResourceHandle - 1])
	{
	    continue;
	}

	auto& viewData = frameResources.ResourceViews[viewIndex];
	viewData       = {};

	if(viewDesc.Image)
	{
	    ImageViewCreateInfo viewCreateInfo {};
	    {
		viewCreateInfo.Image	      = frameResources.Resources[viewDesc.ResourceHandle - 1].Image;
		viewCreateInfo.ViewType	      = viewDesc.ViewType;
		viewCreateInfo.Format	      = viewDesc.Format;
		viewCreateInfo.Components     = viewDesc.Components;
		viewCreateInfo.BaseMipLevel   = viewDesc.SubresourceRange.BaseMipLevel;
		viewCreateInfo.LevelCount     = viewDesc.SubresourceRange.LevelCount;
		viewCreateInfo.BaseArrayLayer = viewDesc.SubresourceRange.BaseArrayLayer;
		viewCreateInfo.LayerCount     = viewDesc.SubresourceRange.LayerCount;
	    }

	    m_adapter->CreateImageView(&viewCreateInfo, &viewData.ImageView);
	    m_adapter->SetDebugObjectName(ObjectType::IMAGE_VIEW, viewData.ImageView, viewDesc.Name);

	    const auto usageFlags = viewCreateInfo.Image->GetDescription().UsageFlags;

	    if((usageFlags & ImageUsageFlags::TEXTURE_BIT) != 0)
	    {
		viewData.TextureHandle = m_resourceViewRegistry->CreateTextureViewHandle(viewData.ImageView, true);
	    }
	    if((usageFlags & ImageUsageFlags::RW_TEXTURE_BIT) != 0)
	    {
		viewData.RWTextureHandle = m_resourceViewRegistry->CreateRWTextureViewHandle(viewData.ImageView, true);
	    }
	}
	else if(viewDesc.Format != Format::UNDEFINED)
	{
	    BufferViewCreateInfo viewCreateInfo = {};
	    {
		viewCreateInfo.Buffer = frameResources.Resources[viewDesc.ResourceHandle - 1].Buffer;
		viewCreateInfo.Format = viewDesc.Format;
		viewCreateInfo.Offset = viewDesc.Offset;
		viewCreateInfo.Range  = viewDesc.Range;
	    }

	    m_adapter->CreateBufferView(&viewCreateInfo, &viewData.BufferView);
	    m_adapter->SetDebugObjectName(ObjectType::BUFFER_VIEW, viewData.BufferView, viewDesc.Name);
	}

	if(!viewDesc.Image)
	{
	    auto buffer			    = frameResources.Resources[viewDesc.ResourceHandle - 1].Buffer;
	    const auto usageFlags	    = buffer->GetDescription().UsageFlags;
	    const auto& resDesc		    = m_resourceDescriptions[viewDesc.ResourceHandle - 1];
	    DescriptorBufferInfo bufferInfo = { buffer, resDesc.Offset + viewDesc.Offset, viewDesc.Range, viewDesc.StructureByteStride };

	    if((usageFlags & BufferUsageFlags::TYPED_BUFFER_BIT) != 0 && viewData.BufferView)
	    {
		viewData.TypedBufferHandle = m_resourceViewRegistry->CreateTypedBufferViewHandle(viewData.BufferView, true);
	    }
	    if((usageFlags & BufferUsageFlags::RW_TYPED_BUFFER_BIT) != 0 && viewData.BufferView)
	    {
		viewData.RWTypedBufferHandle = m_resourceViewRegistry->CreateRWTypedBufferViewHandle(viewData.BufferView, true);
	    }
	    if((usageFlags & BufferUsageFlags::BYTE_BUFFER_BIT) != 0)
	    {
		viewData.ByteBufferHandle = m_resourceViewRegistry->CreateByteBufferViewHandle(bufferInfo, true);
	    }
	    if((usageFlags & BufferUsageFlags::RW_BYTE_BUFFER_BIT) != 0)
	    {
		viewData.RWByteBufferHandle = m_resourceViewRegistry->CreateRWByteBufferViewHandle(bufferInfo, true);
	    }
	    if(viewDesc.StructureByteStride != 0 && (usageFlags & BufferUsageFlags::STRUCTURED_BUFFER_BIT) != 0)
	    {
		viewData.StructuredBufferHandle = m_resourceViewRegistry->CreateStructuredBufferViewHandle(bufferInfo, true);
	    }
	    if(viewDesc.StructureByteStride != 0 && (usageFlags & BufferUsageFlags::RW_STRUCTURED_BUFFER_BIT) != 0)
	    {
		viewData.RWStructuredBufferHandle = m_resourceViewRegistry->CreateRWStructuredBufferViewHandle(bufferInfo, true);
	    }
	}
    }
}

void RenderGraph::CreateSynchronization() noexcept
{
    struct SemaphoreDependencyInfo
    {
	PipelineStageFlags m_waitDstStageMasks[3] = {};
	uint64_t m_waitValues[3]		  = {};
    };

    struct UsageInfo
    {
	uint16_t m_passHandle;
	Queue* m_queue;
	ResourceStateAndStage m_stateAndStage;
    };

    auto& frameResources = m_frameResources[m_frame % FRAME_COUNT];

    eastl::vector<SemaphoreDependencyInfo> semaphoreDependencies(m_passData.size());

    // for each resource...
    const size_t resourceCount = m_resourceDescriptions.size();
    for(size_t resourceIdx = 0; resourceIdx < resourceCount; ++resourceIdx)
    {
	// skip culled resources
	if(m_culledResources[resourceIdx])
	{
	    continue;
	}

	const auto& resDesc = m_resourceDescriptions[resourceIdx];

	// for each subresource...
	const size_t subresourceCount = resDesc.SubresourceCount;
	for(size_t subresourceIdx = 0; subresourceIdx < subresourceCount; ++subresourceIdx)
	{
	    const size_t subresourceUsageIdx = subresourceIdx + resDesc.SubresourceUsageInfoOffset;

	    // check if subresource is used at all
	    if(m_subresourceUsages[subresourceUsageIdx].empty())
	    {
		continue;
	    }

	    UsageInfo prevUsageInfo {};
	    prevUsageInfo.m_queue = m_passData[m_subresourceUsages[subresourceUsageIdx][0].PassHandle].Queue;

	    // if the resource is external, change prevUsageInfo accordingly and update external info values
	    if(resDesc.External)
	    {
		const auto& extInfo	      = resDesc.ExternalStateData;
		prevUsageInfo.m_queue	      = extInfo && extInfo[subresourceIdx].Queue ? extInfo[subresourceIdx].Queue : prevUsageInfo.m_queue;
		prevUsageInfo.m_stateAndStage = extInfo ? extInfo[subresourceIdx].StateAndStage : prevUsageInfo.m_stateAndStage;
		prevUsageInfo.m_passHandle    = prevUsageInfo.m_queue == m_queues[0] ? 0 : prevUsageInfo.m_queue == m_queues[1] ? 1 :
																  2;

		// update external info values
		if(extInfo)
		{
		    const auto& lastUsage		  = m_subresourceUsages[subresourceUsageIdx].back();
		    extInfo[subresourceIdx].Queue	  = m_passData[lastUsage.PassHandle].Queue;
		    extInfo[subresourceIdx].StateAndStage = lastUsage.FinalResourceState;
		}
	    }

	    // for each usage...
	    const size_t usageCount = m_subresourceUsages[subresourceUsageIdx].size();
	    for(size_t usageIdx = 0; usageIdx < usageCount;)
	    {
		const auto& subresUsage = m_subresourceUsages[subresourceUsageIdx][usageIdx];
		auto& passData		= m_passData[subresUsage.PassHandle];

		UsageInfo curUsageInfo { subresUsage.PassHandle, passData.Queue, subresUsage.InitialResourceState };

		// look ahead and try to combine READ_* states
		size_t nextUsageIdx			 = usageIdx + 1;
		ResourceState combinableImageReadStates	 = ResourceState::READ_RESOURCE | ResourceState::READ_DEPTH_STENCIL;
		ResourceState combinableBufferReadStates = ResourceState::READ_RESOURCE | ResourceState::READ_CONSTANT_BUFFER | ResourceState::READ_VERTEX_BUFFER | ResourceState::READ_INDEX_BUFFER | ResourceState::READ_INDIRECT_BUFFER | ResourceState::READ_TRANSFER;

		const bool hasNoCustomFinalState = curUsageInfo.m_stateAndStage.ResourceState == subresUsage.FinalResourceState.ResourceState &&
						   curUsageInfo.m_stateAndStage.StageMask == subresUsage.FinalResourceState.StageMask;

		// is the current state even READ combinable? custom final state breaks this too

		if(hasNoCustomFinalState &&
		   (resDesc.Image && (curUsageInfo.m_stateAndStage.ResourceState & combinableImageReadStates) != 0 ||
		    !resDesc.Image && (curUsageInfo.m_stateAndStage.ResourceState & combinableBufferReadStates) != 0))
		{
		    for(; nextUsageIdx < usageCount; ++nextUsageIdx)
		    {
			const auto& nextSubresUsage = m_subresourceUsages[subresourceUsageIdx][nextUsageIdx];
			auto& nextPassData	    = m_passData[nextSubresUsage.PassHandle];
			UsageInfo nextUsageInfo { nextSubresUsage.PassHandle, nextPassData.Queue, nextSubresUsage.InitialResourceState };

			const bool sameQueue = nextPassData.Queue == passData.Queue;

			const bool noCustomFinalState = nextUsageInfo.m_stateAndStage.ResourceState == nextSubresUsage.FinalResourceState.ResourceState &&
							nextUsageInfo.m_stateAndStage.StageMask == nextSubresUsage.FinalResourceState.StageMask;

			const bool combinable = (resDesc.Image && (nextUsageInfo.m_stateAndStage.ResourceState & combinableImageReadStates) != 0 ||
						 !resDesc.Image && (nextUsageInfo.m_stateAndStage.ResourceState & combinableBufferReadStates) != 0);

			if(!sameQueue || !combinable || !noCustomFinalState)
			{
			    break;
			}

			curUsageInfo.m_stateAndStage.StageMask |= nextUsageInfo.m_stateAndStage.StageMask;
			curUsageInfo.m_stateAndStage.ResourceState |= nextUsageInfo.m_stateAndStage.ResourceState;
		    }
		}

		Barrier barrier {};
		barrier.m_image			= resDesc.Image ? frameResources.Resources[resourceIdx].Image : nullptr;
		barrier.m_buffer		= !resDesc.Image ? frameResources.Resources[resourceIdx].Buffer : nullptr;
		barrier.m_stagesBefore		= prevUsageInfo.m_stateAndStage.StageMask;
		barrier.m_stagesAfter		= curUsageInfo.m_stateAndStage.StageMask;
		barrier.m_stateBefore		= prevUsageInfo.m_stateAndStage.ResourceState;
		barrier.m_stateAfter		= curUsageInfo.m_stateAndStage.ResourceState;
		barrier.m_srcQueue		= prevUsageInfo.m_queue;
		barrier.m_dstQueue		= curUsageInfo.m_queue;
		barrier.m_imageSubresourceRange = { static_cast<uint32_t>(subresourceIdx) % resDesc.Levels, 1, static_cast<uint32_t>(subresourceIdx) / resDesc.Levels, 1 };
		if(barrier.m_srcQueue != barrier.m_dstQueue && !resDesc.Concurrent)
		{
		    barrier.m_flags |= BarrierFlags::QUEUE_OWNERSHIP_AQUIRE;
		}
		if(usageIdx == 0)
		{
		    barrier.m_flags |= BarrierFlags::FIRST_ACCESS_IN_SUBMISSION;
		}

		// split barriers
		if(usageIdx > 0 && prevUsageInfo.m_queue == curUsageInfo.m_queue)
		{
		    // multiple previous usages may have been merged, so we need to insert the begin-split-barrier at the end of the merged batch
		    auto actualPrevPassHandle = m_subresourceUsages[subresourceUsageIdx][usageIdx - 1].PassHandle;
		    if((actualPrevPassHandle + 1) < curUsageInfo.m_passHandle)
		    {
			auto flags = barrier.m_flags;
			barrier.m_flags |= BarrierFlags::BARRIER_BEGIN;
			m_passData[actualPrevPassHandle + 1].BeforeBarriers.push_back(barrier);

			barrier.m_flags = flags;
			barrier.m_flags |= BarrierFlags::BARRIER_END;
		    }
		}

		passData.BeforeBarriers.push_back(barrier);

		const size_t prevQueueIdx = prevUsageInfo.m_queue == m_queues[0] ? 0 : prevUsageInfo.m_queue == m_queues[1] ? 1 :
															      2;

		// we just acquired ownership of the resource -> add a release barrier on the previous queue
		if((barrier.m_flags & BarrierFlags::QUEUE_OWNERSHIP_AQUIRE) != 0)
		{
		    barrier.m_flags ^= BarrierFlags::QUEUE_OWNERSHIP_AQUIRE;
		    barrier.m_flags |= BarrierFlags::QUEUE_OWNERSHIP_RELEASE;

		    semaphoreDependencies[curUsageInfo.m_passHandle].m_waitDstStageMasks[prevQueueIdx] |= curUsageInfo.m_stateAndStage.StageMask;
		    auto& waitValue = semaphoreDependencies[curUsageInfo.m_passHandle].m_waitValues[prevQueueIdx];

		    // external dependency
		    if(usageIdx == 0)
		    {
			m_externalReleaseBarriers[prevUsageInfo.m_passHandle].push_back(barrier);
			waitValue = eastl::max<uint64_t>(*m_semaphoreValues[prevQueueIdx] + 1, waitValue);
		    }
		    else
		    {
			auto& prevPassRecordInfo = m_passData[prevUsageInfo.m_passHandle];
			prevPassRecordInfo.AfterBarriers.push_back(barrier);
			waitValue = eastl::max<uint64_t>(*m_semaphoreValues[prevQueueIdx] + 1 + prevPassRecordInfo.SignalValue, waitValue);
		    }
		}

		// update prevUsageInfo
		prevUsageInfo = curUsageInfo;
		if(!hasNoCustomFinalState)
		{
		    prevUsageInfo.m_stateAndStage = subresUsage.FinalResourceState;
		}

		usageIdx = nextUsageIdx;
	    }
	}
    }

    // create batches
    Queue* prevQueue   = nullptr;
    bool startNewBatch = true;
    for(size_t i = 0; i < m_passData.size(); ++i)
    {
	const auto passHandle		= i;
	const auto& semaphoreDependency = semaphoreDependencies[passHandle];
	Queue* curQueue			= m_passData[passHandle].Queue;

	// if the previous pass needs to signal, startNewBatch is already true
	// if the queue type changed, we need to start a new batch
	// if this pass needs to wait, we also need a need batch
	startNewBatch = startNewBatch || prevQueue != curQueue || semaphoreDependency.m_waitDstStageMasks[0] != 0 || semaphoreDependency.m_waitDstStageMasks[1] != 0 || semaphoreDependency.m_waitDstStageMasks[2] != 0;
	if(startNewBatch)
	{
	    startNewBatch = false;
	    m_recordBatches.push_back({});
	    auto& batch		  = m_recordBatches.back();
	    batch.Queue		  = curQueue;
	    batch.PassIndexOffset = static_cast<uint16_t>(i);
	    for(size_t j = 0; j < 3; ++j)
	    {
		batch.WaitDstStageMasks[j] = semaphoreDependency.m_waitDstStageMasks[j];
		batch.WaitValues[j]	   = semaphoreDependency.m_waitValues[j];
	    }
	}

	// some other passes needs to wait on this one or this is the last pass -> end batch after this pass
	if(!m_passData[passHandle].AfterBarriers.empty() || i == m_passData.size() - 1)
	{
	    startNewBatch = true;
	}

	// update signal value to current last pass in batch
	auto& batch	      = m_recordBatches.back();
	const size_t queueIdx = curQueue == m_queues[0] ? 0 : curQueue == m_queues[1] ? 1 :
											2;
	batch.SignalValue     = MAX(*m_semaphoreValues[queueIdx] + 1 + m_passData[passHandle].SignalValue, batch.SignalValue);

	prevQueue = curQueue;
	++batch.PassIndexCount;
    }
}

void RenderGraph::RecordAndSubmit() noexcept
{
    auto& frameResources	      = m_frameResources[m_frame % FRAME_COUNT];
    frameResources.FinalWaitValues[0] = *m_semaphoreValues[0];
    frameResources.FinalWaitValues[1] = *m_semaphoreValues[1];
    frameResources.FinalWaitValues[2] = *m_semaphoreValues[2];

    // issue release queue ownership transfer barriers for external resources on the wrong queue
    for(size_t i = 0; i < 3; ++i)
    {
	const char* releasePassNames[] {
	    "release barriers for external resources (graphics queue)",
	    "release barriers for external resources (compute queue)",
	    "release barriers for external resources (transfer queue)",
	};

	if(!m_externalReleaseBarriers[i].empty())
	{
	    Command* cmdList = frameResources.CommandFramePool.Acquire(m_queues[i]);

	    cmdList->Begin();

	    //cmdList->InsertDebugLabel(releasePassNames[i]); // TODO
	    cmdList->Barrier(static_cast<uint32_t>(m_externalReleaseBarriers[i].size()), m_externalReleaseBarriers[i].data());

	    cmdList->End();

	    // submit to queue
	    {
		uint64_t waitValue    = *m_semaphoreValues[i];
		uint64_t signalValue  = waitValue + 1;
		auto waitDstStageMask = PipelineStageFlags::TOP_OF_PIPE_BIT;

		SubmitInfo submitInfo {};
		submitInfo.WaitSemaphoreCount	= 1;
		submitInfo.WaitSemaphores	= &m_semaphores[i];
		submitInfo.WaitValues		= &waitValue;
		submitInfo.WaitDstStageMask	= &waitDstStageMask;
		submitInfo.CommandCount		= 1;
		submitInfo.Commands		= &cmdList;
		submitInfo.SignalSemaphoreCount = 1;
		submitInfo.SignalSemaphores	= &m_semaphores[i];
		submitInfo.SignalValues		= &signalValue;

		m_queues[i]->Submit(1, &submitInfo);
		frameResources.FinalWaitValues[i] = eastl::max<uint64_t>(frameResources.FinalWaitValues[i], signalValue);
	    }
	}
    }

    for(const auto& batch: m_recordBatches)
    {
	// allocate command list
	Command* cmdList = frameResources.CommandFramePool.Acquire(batch.Queue);

	cmdList->Begin();

	// record passes
	for(size_t i = 0; i < batch.PassIndexCount; ++i)
	{
	    const auto& passData = m_passData[i + batch.PassIndexOffset];

	    //ScopedLabel debugLabel(cmdList, passData.m_name);

	    // before-barriers
	    if(!passData.BeforeBarriers.empty())
	    {
		cmdList->Barrier(static_cast<uint32_t>(passData.BeforeBarriers.size()), passData.BeforeBarriers.data());
	    }

	    // record commands
	    passData.RecordFunc(cmdList, Registry(this));

	    // after-barriers
	    if(!passData.AfterBarriers.empty())
	    {
		cmdList->Barrier(static_cast<uint32_t>(passData.AfterBarriers.size()), passData.AfterBarriers.data());
	    }
	}

	cmdList->End();

	// submit to queue
	{
	    Semaphore* waitSemaphores[3]	    = {};
	    PipelineStageFlags waitDstStageMasks[3] = {};
	    uint64_t waitValues[3]		    = {};
	    uint32_t waitCount			    = 0;
	    for(size_t i = 0; i < 3; ++i)
	    {
		if(batch.WaitDstStageMasks[i] != 0)
		{
		    waitSemaphores[waitCount]	 = m_semaphores[i];
		    waitDstStageMasks[waitCount] = batch.WaitDstStageMasks[i];
		    waitValues[waitCount]	 = batch.WaitValues[i];
		    ++waitCount;
		}
	    }

	    const size_t queueIdx = batch.Queue == m_queues[0] ? 0 : batch.Queue == m_queues[1] ? 1 :
												  2;

	    SubmitInfo submitInfo = {};
	    {
		submitInfo.WaitSemaphoreCount	= waitCount;
		submitInfo.WaitSemaphores	= waitCount > 0 ? waitSemaphores : nullptr;
		submitInfo.WaitValues		= waitValues;
		submitInfo.WaitDstStageMask	= waitDstStageMasks;
		submitInfo.CommandCount		= 1;
		submitInfo.Commands		= &cmdList;
		submitInfo.SignalSemaphoreCount = 1;
		submitInfo.SignalSemaphores	= &m_semaphores[queueIdx];
		submitInfo.SignalValues		= &batch.SignalValue;
	    }

	    batch.Queue->Submit(1, &submitInfo);

	    frameResources.FinalWaitValues[queueIdx] = MAX(frameResources.FinalWaitValues[queueIdx], batch.SignalValue);
	}
    }

    // update global semaphore values
    *m_semaphoreValues[0] = frameResources.FinalWaitValues[0];
    *m_semaphoreValues[1] = frameResources.FinalWaitValues[1];
    *m_semaphoreValues[2] = frameResources.FinalWaitValues[2];
}
