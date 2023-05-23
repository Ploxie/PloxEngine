//
// Created by Ploxie on 2023-05-11.
//

#pragma once
#include "rendering/types/DescriptorSet.h"
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

class VulkanImageView : public ImageView
{
public:
    explicit VulkanImageView(VkDevice device, const ImageViewCreateInfo& createInfo);
    ~VulkanImageView() override;

    VulkanImageView(VulkanImageView&)			= delete;
    VulkanImageView(VulkanImageView&&)			= delete;
    VulkanImageView& operator=(const VulkanImageView&)	= delete;
    VulkanImageView& operator=(const VulkanImageView&&) = delete;

    void* GetNativeHandle() const override;
    const Image* GetImage() const override;
    const ImageViewCreateInfo& GetDescription() const override;

private:
    VkDevice m_device;
    VkImageView m_imageView;
    ImageViewCreateInfo m_description;
};