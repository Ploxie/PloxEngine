//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "rendering/types/Semaphore.h"
#include "vulkan/vulkan.h"

class VulkanSemaphore : public Semaphore
{
public:
    explicit VulkanSemaphore(VkDevice device, uint64_t initialValue);
    ~VulkanSemaphore();

    void* GetNativeHandle() const override;
    uint64_t GetCompletedValue() const override;
    void Wait(uint64_t waitValue) const override;
    void Signal(uint64_t signalValue) const override;

private:
    VkDevice m_device;
    VkSemaphore m_semaphore;
};