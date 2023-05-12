//
// Created by Ploxie on 2023-05-11.
//

#include "VulkanImage.h"

VulkanImage::VulkanImage(VkImage image, void* allocHandle, const ImageCreateInfo& createInfo)
    : m_image(image), m_allocHandle(allocHandle), m_description(createInfo)
{
}

void* VulkanImage::GetNativeHandle() const
{
    return m_image;
}

const ImageCreateInfo& VulkanImage::GetDescription() const
{
    return m_description;
}

void* VulkanImage::GetAllocationHandle()
{
    return m_allocHandle;
}