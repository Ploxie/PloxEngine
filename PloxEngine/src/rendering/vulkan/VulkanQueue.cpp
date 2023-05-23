//
// Created by Ploxie on 2023-05-10.
//
#include "VulkanQueue.h"
#include "volk.h"
#include "VulkanSemaphore.h"
#include "VulkanUtilities.h"

void *VulkanQueue::GetNativeHandle() const
{
    return m_queue;
}

VkQueue *VulkanQueue::GetQueue()
{
    return &m_queue;
}

VulkanQueue::QueueType VulkanQueue::GetQueueType() const
{
    return m_queueType;
}

unsigned int VulkanQueue::GetTimestampValidBits() const
{
    return m_timestampValidBits;
}

float VulkanQueue::GetTimestampPeriod() const
{
    return m_timestampPeriod;
}

bool VulkanQueue::CanPresent() const
{
    return m_presentable;
}

void VulkanQueue::Submit(uint32_t count, const SubmitInfo *submitInfo)
{
    // compute number of involved semaphores (both wait and signal), number of command buffers and number of wait dst stage masks
    size_t semaphoreCount     = 0;
    size_t commandBufferCount = 0;
    size_t waitMaskCount      = 0;
    for(uint32_t i = 0; i < count; ++i)
    {
	semaphoreCount += submitInfo[i].WaitSemaphoreCount + submitInfo[i].SignalSemaphoreCount;
	waitMaskCount += submitInfo[i].WaitSemaphoreCount;
	commandBufferCount += submitInfo[i].CommandCount;
    }

    // allocate arrays
    VkSubmitInfo *submitInfoVk				   = STACK_ALLOC_T(VkSubmitInfo, count);
    VkTimelineSemaphoreSubmitInfo *timelineSemaphoreInfoVk = STACK_ALLOC_T(VkTimelineSemaphoreSubmitInfo, count);
    VkSemaphore *semaphoresVk				   = STACK_ALLOC_T(VkSemaphore, semaphoreCount);
    VkPipelineStageFlags *waitDstStageMasksVk		   = STACK_ALLOC_T(VkPipelineStageFlags, waitMaskCount);
    VkCommandBuffer *commandBuffersVk			   = STACK_ALLOC_T(VkCommandBuffer, commandBufferCount);

    // keep track of the current offset into the secondary arrays. submitInfoVk and timelineSemaphoreInfoVk can be indexed with i
    size_t semaphoreCurOffset	   = 0;
    size_t commandBuffersCurOffset = 0;
    size_t waitMaskCurOffset	   = 0;

    for(uint32_t i = 0; i < count; ++i)
    {
	// store the base offset into the secondary arrays for this submit info
	const size_t waitSemaphoreSubmitInfoOffset  = semaphoreCurOffset;
	const size_t commandBuffersSubmitInfoOffset = commandBuffersCurOffset;
	const size_t waitMaskSubmitInfoOffset	    = waitMaskCurOffset;

	// copy wait semaphores and wait stage masks
	for(uint32_t j = 0; j < submitInfo[i].WaitSemaphoreCount; ++j)
	{
	    semaphoresVk[semaphoreCurOffset++]	     = (VkSemaphore) submitInfo[i].WaitSemaphores[j]->GetNativeHandle();
	    waitDstStageMasksVk[waitMaskCurOffset++] = VulkanUtilities::Translate(submitInfo[i].WaitDstStageMask[j]);
	}

	// copy command buffers
	for(uint32_t j = 0; j < submitInfo[i].CommandCount; ++j)
	{
	    commandBuffersVk[commandBuffersCurOffset++] = (VkCommandBuffer) submitInfo[i].Commands[j]->GetNativeHandle();
	}

	// store the base offset into the semaphore array for signal semaphores (both wait and signal semaphores share the same array)
	const size_t signalSemaphoreSubmitInfoOffset = semaphoreCurOffset;

	// copy signal semaphores
	for(uint32_t j = 0; j < submitInfo[i].SignalSemaphoreCount; ++j)
	{
	    semaphoresVk[semaphoreCurOffset++] = (VkSemaphore) submitInfo[i].SignalSemaphores[j]->GetNativeHandle();
	}

	auto &timelineSubInfo			  = timelineSemaphoreInfoVk[i];
	timelineSubInfo				  = { VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
	timelineSubInfo.waitSemaphoreValueCount	  = submitInfo[i].WaitSemaphoreCount;
	timelineSubInfo.pWaitSemaphoreValues	  = submitInfo[i].WaitValues;
	timelineSubInfo.signalSemaphoreValueCount = submitInfo[i].SignalSemaphoreCount;
	timelineSubInfo.pSignalSemaphoreValues	  = submitInfo[i].SignalValues;

	auto &subInfo		     = submitInfoVk[i];
	subInfo			     = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	subInfo.pNext		     = &timelineSubInfo;
	subInfo.waitSemaphoreCount   = submitInfo[i].WaitSemaphoreCount;
	subInfo.pWaitSemaphores	     = semaphoresVk + waitSemaphoreSubmitInfoOffset;
	subInfo.pWaitDstStageMask    = waitDstStageMasksVk + waitMaskSubmitInfoOffset;
	subInfo.commandBufferCount   = submitInfo[i].CommandCount;
	subInfo.pCommandBuffers	     = commandBuffersVk + commandBuffersSubmitInfoOffset;
	subInfo.signalSemaphoreCount = submitInfo[i].SignalSemaphoreCount;
	subInfo.pSignalSemaphores    = semaphoresVk + signalSemaphoreSubmitInfoOffset;
    }

    VulkanUtilities::checkResult(vkQueueSubmit(m_queue, count, submitInfoVk, VK_NULL_HANDLE), "Failed to submit to Queue!");
}

unsigned int VulkanQueue::GetQueueFamily() const
{
    return m_queueFamily;
}