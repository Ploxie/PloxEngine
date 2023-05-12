
//
// Created by Ploxie on 2023-05-11.
//
#include "vulkansemaphore.h"
#include "VulkanUtilities.h"

VulkanSemaphore::VulkanSemaphore(VkDevice device, uint64_t initialValue)
    : m_device(device), m_semaphore(VK_NULL_HANDLE)
{
    VkSemaphoreTypeCreateInfo typeCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO };
    {
	typeCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
	typeCreateInfo.initialValue  = initialValue;
    }

    VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, &typeCreateInfo };

    VulkanUtilities::checkResult(vkCreateSemaphore(m_device, &createInfo, nullptr, &m_semaphore));
}

VulkanSemaphore::~VulkanSemaphore()
{
    vkDestroySemaphore(m_device, m_semaphore, nullptr);
}

void* VulkanSemaphore::GetNativeHandle() const
{
    return m_semaphore;
}

uint64_t VulkanSemaphore::GetCompletedValue() const
{
    uint64_t value = 0;
    VulkanUtilities::checkResult(vkGetSemaphoreCounterValue(m_device, m_semaphore, &value));
    return value;
}

void VulkanSemaphore::Wait(uint64_t waitValue) const
{
    VkSemaphoreWaitInfo waitInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO };
    {
	waitInfo.flags		= VK_SEMAPHORE_WAIT_ANY_BIT;
	waitInfo.semaphoreCount = 1;
	waitInfo.pSemaphores	= &m_semaphore;
	waitInfo.pValues	= &waitValue;
    }

    VulkanUtilities::checkResult(vkWaitSemaphores(m_device, &waitInfo, UINT64_MAX));
}

void VulkanSemaphore::Signal(uint64_t signalValue) const
{
    VkSemaphoreSignalInfo signalInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO };
    {
	signalInfo.semaphore = m_semaphore;
	signalInfo.value     = signalValue;
    }

    VulkanUtilities::checkResult(vkSignalSemaphore(m_device, &signalInfo));
}
