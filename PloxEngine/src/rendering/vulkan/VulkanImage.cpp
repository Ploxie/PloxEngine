//
// Created by Ploxie on 2023-05-11.
//

#include "VulkanImage.h"
#include "volk.h"
#include "VulkanUtilities.h"

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

VulkanImageView::VulkanImageView(VkDevice device, const ImageViewCreateInfo& createInfo)
    : m_device(device), m_imageView(VK_NULL_HANDLE), m_description(createInfo)
{
    const auto* image = dynamic_cast<const VulkanImage*>(createInfo.Image);
    ASSERT(image);

    VkImageViewCreateInfo viewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    {
	viewCreateInfo.image			       = static_cast<VkImage>(image->GetNativeHandle());
	viewCreateInfo.viewType			       = VulkanUtilities::Translate(createInfo.ViewType);
	viewCreateInfo.format			       = VulkanUtilities::Translate(createInfo.Format == Format::UNDEFINED ? image->GetDescription().Format : createInfo.Format);
	viewCreateInfo.components.r		       = VulkanUtilities::Translate(createInfo.Components.m_r);
	viewCreateInfo.components.g		       = VulkanUtilities::Translate(createInfo.Components.m_g);
	viewCreateInfo.components.b		       = VulkanUtilities::Translate(createInfo.Components.m_b);
	viewCreateInfo.components.a		       = VulkanUtilities::Translate(createInfo.Components.m_a);
	viewCreateInfo.subresourceRange.aspectMask     = VulkanUtilities::GetImageAspectMask(VulkanUtilities::Translate(image->GetDescription().Format));
	viewCreateInfo.subresourceRange.baseMipLevel   = createInfo.BaseMipLevel;
	viewCreateInfo.subresourceRange.levelCount     = createInfo.LevelCount;
	viewCreateInfo.subresourceRange.baseArrayLayer = createInfo.BaseArrayLayer;
	viewCreateInfo.subresourceRange.layerCount     = createInfo.LayerCount;
    }

    VulkanUtilities::checkResult(vkCreateImageView(m_device, &viewCreateInfo, nullptr, &m_imageView));
}

VulkanImageView::~VulkanImageView()
{
    vkDestroyImageView(m_device, m_imageView, nullptr);
}

void* VulkanImageView::GetNativeHandle() const
{
    return m_imageView;
}

const Image* VulkanImageView::GetImage() const
{
    return m_description.Image;
}

const ImageViewCreateInfo& VulkanImageView::GetDescription() const
{
    return m_description;
}
