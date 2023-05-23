//
// Created by Ploxie on 2023-05-17.
//
#include "VulkanDescriptorSet.h"
#include "core/Assert.h"
#include "EASTL/vector.h"
#include "rendering/RenderUtilities.h"
#include "utility/memory/DefaultAllocator.h"
#include "volk.h"
#include "VulkanBuffer.h"
#include "VulkanUtilities.h"

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDevice device, uint32_t bindingCount, const VkDescriptorSetLayoutBinding* bindings, const VkDescriptorBindingFlags* bindingFlags)
    : m_device(device), m_descriptorSetLayout(VK_NULL_HANDLE), m_typeCounts()
{
    for(uint32_t i = 0; i < bindingCount; i++)
    {
	ASSERT(static_cast<size_t>(bindings[i].descriptorType) < eastl::size(m_typeCounts));
	m_typeCounts[static_cast<size_t>(bindings[i].descriptorType)] += bindings[i].descriptorCount;
    }

    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
    {
	bindingFlagsCreateInfo.bindingCount  = bindingCount;
	bindingFlagsCreateInfo.pBindingFlags = bindingFlags;
    }

    VkDescriptorSetLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    {
	createInfo.pNext	= bindingFlags ? &bindingFlagsCreateInfo : nullptr;
	createInfo.flags	= VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
	createInfo.bindingCount = bindingCount;
	createInfo.pBindings	= bindings;
    }

    VulkanUtilities::checkResult(vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &m_descriptorSetLayout));
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
}

void* VulkanDescriptorSetLayout::GetNativeHandle() const
{
    return m_descriptorSetLayout;
}

const uint32_t* VulkanDescriptorSetLayout::GetTypeCounts() const
{
    return m_typeCounts;
}

VulkanDescriptorSet::VulkanDescriptorSet(VkDevice device, VkDescriptorSet descriptorSet, uint32_t dynamicBufferCount)
    : m_device(device), m_descriptorSet(descriptorSet), m_dynamicBufferCount(dynamicBufferCount)
{
}

void* VulkanDescriptorSet::GetNativeHandle() const
{
    return m_descriptorSet;
}

void VulkanDescriptorSet::Update(uint32_t count, const DescriptorSetUpdate* updates)
{
    constexpr uint32_t BATCH_SIZE = 16;
    const uint32_t iterations	  = (count + (BATCH_SIZE - 1)) / BATCH_SIZE;
    for(uint32_t i = 0; i < iterations; i++)
    {
	const uint32_t countVk = MIN(BATCH_SIZE, count - i * BATCH_SIZE);

	eastl::vector<VkDescriptorImageInfo> imageInfos;
	eastl::vector<VkDescriptorBufferInfo> bufferInfos;
	eastl::vector<VkBufferView> texelBufferViews;

	size_t imageInfoReserveCount	    = 0;
	size_t bufferInfoReserveCount	    = 0;
	size_t texelBufferViewsReserveCount = 0;
	for(uint32_t j = 0; j < countVk; j++)
	{
	    const auto& update = updates[i * BATCH_SIZE + j];
	    switch(update.DescriptorType)
	    {
		case DescriptorType::SAMPLER:
		case DescriptorType::TEXTURE:
		case DescriptorType::RW_TEXTURE: imageInfoReserveCount += update.DescriptorCount; break;
		case DescriptorType::TYPED_BUFFER:
		case DescriptorType::RW_TYPED_BUFFER: texelBufferViewsReserveCount += update.DescriptorCount; break;
		case DescriptorType::CONSTANT_BUFFER:
		case DescriptorType::BYTE_BUFFER:
		case DescriptorType::RW_BYTE_BUFFER:
		case DescriptorType::STRUCTURED_BUFFER:
		case DescriptorType::RW_STRUCTURED_BUFFER: bufferInfoReserveCount += update.DescriptorCount; break;
		case DescriptorType::OFFSET_CONSTANT_BUFFER: bufferInfoReserveCount += 1; break;
		default:
		    ASSERT(false)
		    break;
	    }
	}

	imageInfos.reserve(imageInfoReserveCount);
	bufferInfos.reserve(bufferInfoReserveCount);
	texelBufferViews.reserve(texelBufferViewsReserveCount);

	VkWriteDescriptorSet writesVk[BATCH_SIZE];
	for(uint32_t j = 0; j < countVk; j++)
	{
	    const auto& update = updates[i * BATCH_SIZE + j];
	    auto& write	       = writesVk[j];

	    write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	    {
		write.dstSet	      = m_descriptorSet;
		write.dstBinding      = update.DstBinding;
		write.dstArrayElement = update.DstArrayElement;
		write.descriptorCount = update.DescriptorCount;
	    }

	    switch(update.DescriptorType)
	    {
		case DescriptorType::SAMPLER:
		{
		    for(size_t k = 0; k < update.DescriptorCount; k++)
		    {
			const Sampler* sampler = update.Samplers ? update.Samplers[k] : update.Sampler;
			imageInfos.push_back({ static_cast<VkSampler>(sampler->GetNativeHandle()), VK_NULL_HANDLE, VK_IMAGE_LAYOUT_UNDEFINED });
		    }
		    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		    write.pImageInfo	 = imageInfos.data() + imageInfos.size() - update.DescriptorCount;
		    break;
		}
		case DescriptorType::TEXTURE:
		{
		    for(size_t k = 0; k < update.DescriptorCount; k++)
		    {
			const ImageView* view = update.ImageViews ? update.ImageViews[k] : update.ImageView;
			const auto format     = view->GetDescription().Format;
			const auto layout     = RenderUtilities::IsDepthFormat(format) || RenderUtilities::IsStencilFormat(format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfos.push_back({ VK_NULL_HANDLE, static_cast<VkImageView>(view->GetNativeHandle()), layout });
		    }
		    write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		    write.pImageInfo	 = imageInfos.data() + imageInfos.size() - update.DescriptorCount;
		    break;
		}
		case DescriptorType::RW_TEXTURE:
		{
		    for(size_t k = 0; k < update.DescriptorCount; ++k)
		    {
			const ImageView* view = update.ImageViews ? update.ImageViews[k] : update.ImageView;
			imageInfos.push_back({ VK_NULL_HANDLE, static_cast<VkImageView>(view->GetNativeHandle()), VK_IMAGE_LAYOUT_GENERAL });
		    }
		    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		    write.pImageInfo	 = imageInfos.data() + imageInfos.size() - update.DescriptorCount;
		    break;
		}
		case DescriptorType::TYPED_BUFFER:
		{
		    for(size_t k = 0; k < update.DescriptorCount; ++k)
		    {
			const BufferView* view = update.BufferViews ? update.BufferViews[k] : update.BufferView;
			texelBufferViews.push_back(static_cast<VkBufferView>(view->GetNativeHandle()));
		    }
		    write.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		    write.pTexelBufferView = texelBufferViews.data() + texelBufferViews.size() - update.DescriptorCount;
		    break;
		}
		case DescriptorType::RW_TYPED_BUFFER:
		{
		    for(size_t k = 0; k < update.DescriptorCount; ++k)
		    {
			const BufferView* view = update.BufferViews ? update.BufferViews[k] : update.BufferView;
			texelBufferViews.push_back(static_cast<VkBufferView>(view->GetNativeHandle()));
		    }
		    write.descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		    write.pTexelBufferView = texelBufferViews.data() + texelBufferViews.size() - update.DescriptorCount;
		    break;
		}
		case DescriptorType::CONSTANT_BUFFER:
		{
		    for(size_t k = 0; k < update.DescriptorCount; ++k)
		    {
			const auto& info     = update.BufferInfo ? update.BufferInfo[k] : update.BufferInfo1;
			const auto* bufferVk = dynamic_cast<const VulkanBuffer*>(info.Buffer);
			ASSERT(bufferVk);
			bufferInfos.push_back({ static_cast<VkBuffer>(bufferVk->GetNativeHandle()), info.Offset, info.Range });
		    }
		    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		    write.pBufferInfo	 = bufferInfos.data() + bufferInfos.size() - update.DescriptorCount;

		    break;
		}
		case DescriptorType::BYTE_BUFFER:
		case DescriptorType::RW_BYTE_BUFFER:
		case DescriptorType::STRUCTURED_BUFFER:
		case DescriptorType::RW_STRUCTURED_BUFFER:
		{
		    for(size_t k = 0; k < update.DescriptorCount; ++k)
		    {
			const auto& info     = update.BufferInfo ? update.BufferInfo[k] : update.BufferInfo1;
			const auto* bufferVk = dynamic_cast<const VulkanBuffer*>(info.Buffer);
			ASSERT(bufferVk);
			bufferInfos.push_back({ static_cast<VkBuffer>(bufferVk->GetNativeHandle()), info.Offset, info.Range });
		    }
		    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		    write.pBufferInfo	 = bufferInfos.data() + bufferInfos.size() - update.DescriptorCount;

		    break;
		}
		case DescriptorType::OFFSET_CONSTANT_BUFFER:
		{
		    const auto& info	 = update.BufferInfo ? update.BufferInfo[0] : update.BufferInfo1;
		    const auto* bufferVk = dynamic_cast<const VulkanBuffer*>(info.Buffer);
		    ASSERT(bufferVk);
		    bufferInfos.push_back({ static_cast<VkBuffer>(bufferVk->GetNativeHandle()), info.Offset, info.Range });
		    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		    write.pBufferInfo	 = bufferInfos.data() + bufferInfos.size() - 1;

		    break;
		}
		default:
		    ASSERT(false);
		    break;
	    }
	}
    }
}

uint32_t VulkanDescriptorSet::GetDynamicBufferCount() const
{
    return m_dynamicBufferCount;
}
VulkanDescriptorSetPool::VulkanDescriptorSetPool(VkDevice device, uint32_t maxSets, const VulkanDescriptorSetLayout* layout)
    : m_device(device), m_descriptorPool(VK_NULL_HANDLE), m_layout(layout), m_poolSize(maxSets), m_currentOffset(), m_descriptorSetMemory()
{
    constexpr uint32_t DESCRIPTOR_TYPES = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT - VK_DESCRIPTOR_TYPE_SAMPLER + 1;
    VkDescriptorPoolSize poolSizes[DESCRIPTOR_TYPES];

    uint32_t poolSizeCount = 0;

    const uint32_t* typeCounts = layout->GetTypeCounts();

    for(size_t i = 0; i < DESCRIPTOR_TYPES; i++)
    {
	if(typeCounts[i])
	{
	    poolSizes[poolSizeCount].type	     = static_cast<VkDescriptorType>(i);
	    poolSizes[poolSizeCount].descriptorCount = typeCounts[i] * maxSets;
	    poolSizeCount++;
	}
    }

    ASSERT(poolSizeCount);

    VkDescriptorPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    {
	createInfo.flags	 = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
	createInfo.maxSets	 = maxSets;
	createInfo.poolSizeCount = poolSizeCount;
	createInfo.pPoolSizes	 = poolSizes;
    }

    VulkanUtilities::checkResult(vkCreateDescriptorPool(m_device, &createInfo, nullptr, &m_descriptorPool));

    m_descriptorSetMemory = static_cast<char*>(DefaultAllocator::Get()->allocate(sizeof(VulkanDescriptorSet) * m_poolSize, alignof(VulkanDescriptorSet), 0));
}
VulkanDescriptorSetPool::~VulkanDescriptorSetPool()
{
    vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);

    DefaultAllocator::Get()->deallocate(m_descriptorSetMemory, sizeof(VulkanDescriptorSet) * m_poolSize);
    delete m_descriptorSetMemory;
    m_descriptorSetMemory = nullptr;
}
void* VulkanDescriptorSetPool::GetNativeHandle() const
{
    return m_descriptorPool;
}
void VulkanDescriptorSetPool::AllocateDescriptorSets(uint32_t count, DescriptorSet** sets)
{
    ASSERT_MSG(m_currentOffset + count > m_poolSize, "Tried to allocate more descriptor sets from descriptor set pool than available!")

    auto* layout		      = static_cast<VkDescriptorSetLayout>(m_layout->GetNativeHandle());
    const uint32_t dynamicBufferCount = m_layout->GetTypeCounts()[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC];

    constexpr uint32_t BATCH_SIZE = 8;
    const uint32_t iterations	  = (count + (BATCH_SIZE - 1)) / BATCH_SIZE;
    for(uint32_t i = 0; i < iterations; i++)
    {
	const uint32_t countVk = MIN(BATCH_SIZE, count - i * BATCH_SIZE);
	VkDescriptorSetLayout layouts[BATCH_SIZE];
	for(uint32_t j = 0; j < countVk; j++)
	{
	    layouts[j] = layout;
	}

	VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	{
	    allocInfo.descriptorPool	 = m_descriptorPool;
	    allocInfo.descriptorSetCount = countVk;
	    allocInfo.pSetLayouts	 = layouts;
	}

	VkDescriptorSet setsVk[BATCH_SIZE];

	VulkanUtilities::checkResult(vkAllocateDescriptorSets(m_device, &allocInfo, setsVk));

	for(uint32_t j = 0; j < countVk; j++)
	{
	    sets[i * BATCH_SIZE + j] = new(m_descriptorSetMemory + sizeof(VulkanDescriptorSet) * m_currentOffset) VulkanDescriptorSet(m_device, setsVk[j], dynamicBufferCount);
	    m_currentOffset++;
	}
    }
}
void VulkanDescriptorSetPool::Reset()
{
    VulkanUtilities::checkResult(vkResetDescriptorPool(m_device, m_descriptorPool, 0));

    m_currentOffset = 0;
}
