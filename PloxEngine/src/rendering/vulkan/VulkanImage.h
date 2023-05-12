//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "rendering/types/Image.h"
#include "vulkan/vulkan.h"

class VulkanImage : public Image
{
public:
    explicit VulkanImage(VkImage image, void* allocHandle, const ImageCreateInfo& createInfo);
    void* GetNativeHandle() const override;
    const ImageCreateInfo& GetDescription() const override;
    void* GetAllocationHandle();

private:
    VkImage m_image;
    void* m_allocHandle;
    ImageCreateInfo m_description;
};