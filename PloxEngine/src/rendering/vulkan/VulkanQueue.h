//
// Created by Ploxie on 2023-05-10.
//
#pragma once
#include "rendering/types/Queue.h"
#include "vulkan/vulkan.h"

class VulkanQueue : public Queue
{
public:
    friend class VulkanGraphicsAdapter;

    void* GetNativeHandle() const override;
    VkQueue* GetQueue();
    QueueType GetQueueType() const override;
    unsigned int GetTimestampValidBits() const override;
    float GetTimestampPeriod() const override;
    bool CanPresent() const override;
    void Submit(uint32_t count, const SubmitInfo* submitInfo) override;
    unsigned int GetQueueFamily() const;

private:
    VkQueue m_queue = VK_NULL_HANDLE;
    QueueType m_queueType;
    uint32_t m_timestampValidBits = 0;
    float m_timestampPeriod	  = 1.0f;
    bool m_presentable		  = false;
    unsigned int m_queueFamily	  = -1;
};
