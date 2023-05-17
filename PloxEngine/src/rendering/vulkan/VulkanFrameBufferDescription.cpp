//
// Created by Ploxie on 2023-05-17.
//

#include "VulkanFrameBufferDescription.h"
#include "core/Assert.h"
#include "utility/Utilities.h"
#include <cstring>

VulkanFrameBufferDescription::VulkanFrameBufferDescription()
{
    memset(this, 0, sizeof(*this));
}
void VulkanFrameBufferDescription::SetRenderPass(VkRenderPass renderPass)
{
    m_renderPass = renderPass;
}
void VulkanFrameBufferDescription::SetAttachment(uint32_t count, const VulkanFrameBufferDescription::AttachmentInfo *attachmentInfos)
{
    ASSERT(count <= 9);
    memcpy(m_attachmentInfos, attachmentInfos, sizeof(m_attachmentInfos[0]) * count);
    m_attachmentCount = count;
}
void VulkanFrameBufferDescription::SetExtent(uint32_t width, uint32_t height, uint32_t layers)
{
    m_width  = width;
    m_height = height;
    m_layers = layers;
}
void VulkanFrameBufferDescription::Finalize()
{
    m_hashValue = 0;

    for(size_t i = 0; (i + 4) <= sizeof(*this); i += 4)
    {
	uint32_t word = *reinterpret_cast<const uint32_t *>(reinterpret_cast<const char *>(this) + i);
	Util::HashCombine(m_hashValue, word);
    }
}

bool operator==(const VulkanFrameBufferDescription &left, const VulkanFrameBufferDescription &right)
{
    return memcmp(&left, &right, sizeof(right)) == 0;
}
