//
// Created by Ploxie on 2023-05-10.
//
#include "VulkanQueue.h"

void* VulkanQueue::GetNativeHandle() const
{
    return m_queue;
}

VkQueue* VulkanQueue::GetQueue()
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

unsigned int VulkanQueue::GetQueueFamily() const
{
    return m_queueFamily;
}