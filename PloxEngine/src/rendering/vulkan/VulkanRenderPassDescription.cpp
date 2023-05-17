//
// Created by Ploxie on 2023-05-17.
//

#include "VulkanRenderPassDescription.h"
#include "core/Assert.h"
#include "utility/Utilities.h"
#include <cstring>

VulkanRenderPassDescription::VulkanRenderPassDescription()
{
    memset(this, 0, sizeof(*this));
}

void VulkanRenderPassDescription::SetColorAttachment(uint32_t count, const VulkanRenderPassDescription::ColorAttachmentDescription *colorAttachments)
{
    ASSERT(count <= 8);
    memcpy(m_colorAttachments, colorAttachments, sizeof(colorAttachments[0]) * count);
    m_colorAttachmentCount = count;
}

void VulkanRenderPassDescription::SetDepthStencilAttachment(const VulkanRenderPassDescription::DepthStencilAttachmentDescription &depthStencilAttachment)
{
    m_depthStencilAttachment	    = depthStencilAttachment;
    m_depthStencilAttachmentPresent = VK_TRUE;
}

void VulkanRenderPassDescription::Finalize()
{
    m_hashValue = 0;
    for(size_t i = 0; (i + 4) <= sizeof(*this); i += 4)
    {
	uint32_t word = *reinterpret_cast<const uint32_t *>(reinterpret_cast<const char *>(this) + i);
	Util::HashCombine(m_hashValue, word);
    }
}

bool operator==(const VulkanRenderPassDescription &left, const VulkanRenderPassDescription &right)
{
    return memcmp(&left, &right, sizeof(right)) == 0;
}