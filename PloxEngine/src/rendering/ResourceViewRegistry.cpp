//
// Created by Ploxie on 2023-05-23.
//
#include "ResourceViewRegistry.h"
#include "GraphicsAdapter.h"
#include "rendering/types/GraphicsPipeline.h"

ResourceViewRegistry::ResourceViewRegistry(GraphicsAdapter* adapter)
    : m_adapter(adapter), m_textureHandleManager(UINT16_MAX)
{
    auto stages	      = ShaderStageFlags::ALL_STAGES;
    auto bindingFlags = DescriptorBindingFlags::UPDATE_AFTER_BIND_BIT | DescriptorBindingFlags::PARTIALLY_BOUND_BIT;

    DescriptorSetLayoutBinding bindings[] = {
	{ DescriptorType::TEXTURE, TEXTURE_BINDING, 0, UINT16_MAX, stages, bindingFlags },
	{ DescriptorType::BYTE_BUFFER, BYTE_BUFFER_BINDING, 0, UINT16_MAX, stages, bindingFlags },
	{ DescriptorType::TYPED_BUFFER, TYPED_BUFFER_BINDING, 0, UINT16_MAX, stages, bindingFlags },
	{ DescriptorType::RW_TEXTURE, RW_TEXTURE_BINDING, 0, UINT16_MAX, stages, bindingFlags },
	{ DescriptorType::RW_TYPED_BUFFER, RW_TYPED_BUFFER_BINDING, 0, UINT16_MAX, stages, bindingFlags },
	{ DescriptorType::RW_BYTE_BUFFER, RW_BYTE_BUFFER_BINDING, 0, UINT16_MAX, stages, bindingFlags },
    };

    m_adapter->CreateDescriptorSetLayout((uint32_t) eastl::size(bindings), bindings, &m_descriptorSetLayout);
    m_adapter->CreateDescriptorSetPool(2, m_descriptorSetLayout, &m_descriptorSetPool);
    m_descriptorSetPool->AllocateDescriptorSets(2, m_descriptorSets);
}

ResourceViewRegistry::~ResourceViewRegistry()
{
    m_adapter->DestroyDescriptorSetPool(m_descriptorSetPool);
    m_adapter->DestroyDescriptorSetLayout(m_descriptorSetLayout);
}

TextureViewHandle ResourceViewRegistry::CreateTextureViewHandle(ImageView* imageView, bool transient) noexcept
{
    return (TextureViewHandle) CreateHandle(m_textureHandleManager, m_textureHandleManagerMutex, TEXTURE_BINDING, transient, DescriptorType::TEXTURE, imageView, nullptr, nullptr);
}

RWTextureViewHandle ResourceViewRegistry::CreateRWTextureViewHandle(ImageView* imageView, bool transient) noexcept
{
    return (RWTextureViewHandle) CreateHandle(m_rwTextureHandleManager, m_rwTextureHandleManagerMutex, RW_TEXTURE_BINDING, transient, DescriptorType::RW_TEXTURE, imageView, nullptr, nullptr);
}

TypedBufferViewHandle ResourceViewRegistry::CreateTypedBufferViewHandle(BufferView* bufferView, bool transient) noexcept
{
    return (TypedBufferViewHandle) CreateHandle(m_typedBufferHandleManager, m_typedBufferHandleManagerMutex, TYPED_BUFFER_BINDING, transient, DescriptorType::TYPED_BUFFER, nullptr, bufferView, nullptr);
}

RWTypedBufferViewHandle ResourceViewRegistry::CreateRWTypedBufferViewHandle(BufferView* bufferView, bool transient) noexcept
{
    return (RWTypedBufferViewHandle) CreateHandle(m_rwTypedBufferHandleManager, m_rwTypedBufferHandleManagerMutex, RW_TYPED_BUFFER_BINDING, transient, DescriptorType::RW_TYPED_BUFFER, nullptr, bufferView, nullptr);
}

ByteBufferViewHandle ResourceViewRegistry::CreateByteBufferViewHandle(const DescriptorBufferInfo& bufferInfo, bool transient) noexcept
{
    return (ByteBufferViewHandle) CreateHandle(m_byteBufferHandleManager, m_byteBufferHandleManagerMutex, BYTE_BUFFER_BINDING, transient, DescriptorType::BYTE_BUFFER, nullptr, nullptr, &bufferInfo);
}

RWByteBufferViewHandle ResourceViewRegistry::CreateRWByteBufferViewHandle(const DescriptorBufferInfo& bufferInfo, bool transient) noexcept
{
    return (RWByteBufferViewHandle) CreateHandle(m_rwByteBufferHandleManager, m_rwByteBufferHandleManagerMutex, RW_BYTE_BUFFER_BINDING, transient, DescriptorType::RW_BYTE_BUFFER, nullptr, nullptr, &bufferInfo);
}

StructuredBufferViewHandle ResourceViewRegistry::CreateStructuredBufferViewHandle(const DescriptorBufferInfo& bufferInfo, bool transient) noexcept
{
    return (StructuredBufferViewHandle) CreateHandle(m_byteBufferHandleManager, m_byteBufferHandleManagerMutex, BYTE_BUFFER_BINDING, transient, DescriptorType::STRUCTURED_BUFFER, nullptr, nullptr, &bufferInfo);
}

RWStructuredBufferViewHandle ResourceViewRegistry::CreateRWStructuredBufferViewHandle(const DescriptorBufferInfo& bufferInfo, bool transient) noexcept
{
    return (RWStructuredBufferViewHandle) CreateHandle(m_rwByteBufferHandleManager, m_rwByteBufferHandleManagerMutex, RW_BYTE_BUFFER_BINDING, transient, DescriptorType::RW_STRUCTURED_BUFFER, nullptr, nullptr, &bufferInfo);
}

void ResourceViewRegistry::UpdateHandle(TextureViewHandle handle, ImageView* imageView) noexcept
{
    UpdateHandle(handle, TEXTURE_BINDING, DescriptorType::TEXTURE, imageView, nullptr, nullptr);
}

void ResourceViewRegistry::UpdateHandle(RWTextureViewHandle handle, ImageView* imageView) noexcept
{
    UpdateHandle(handle, RW_TEXTURE_BINDING, DescriptorType::RW_TEXTURE, imageView, nullptr, nullptr);
}

void ResourceViewRegistry::UpdateHandle(TypedBufferViewHandle handle, BufferView* bufferView) noexcept
{
    UpdateHandle(handle, TYPED_BUFFER_BINDING, DescriptorType::TYPED_BUFFER, nullptr, bufferView, nullptr);
}

void ResourceViewRegistry::UpdateHandle(RWTypedBufferViewHandle handle, BufferView* bufferView) noexcept
{
    UpdateHandle(handle, RW_TYPED_BUFFER_BINDING, DescriptorType::RW_TYPED_BUFFER, nullptr, bufferView, nullptr);
}

void ResourceViewRegistry::UpdateHandle(ByteBufferViewHandle handle, const DescriptorBufferInfo& bufferInfo) noexcept
{
    UpdateHandle(handle, BYTE_BUFFER_BINDING, DescriptorType::BYTE_BUFFER, nullptr, nullptr, &bufferInfo);
}

void ResourceViewRegistry::UpdateHandle(RWByteBufferViewHandle handle, const DescriptorBufferInfo& bufferInfo) noexcept
{
    UpdateHandle(handle, RW_BYTE_BUFFER_BINDING, DescriptorType::RW_BYTE_BUFFER, nullptr, nullptr, &bufferInfo);
}

void ResourceViewRegistry::UpdateHandle(StructuredBufferViewHandle handle, const DescriptorBufferInfo& bufferInfo) noexcept
{
    UpdateHandle(handle, BYTE_BUFFER_BINDING, DescriptorType::STRUCTURED_BUFFER, nullptr, nullptr, &bufferInfo);
}

void ResourceViewRegistry::UpdateHandle(RWStructuredBufferViewHandle handle, const DescriptorBufferInfo& bufferInfo) noexcept
{
    UpdateHandle(handle, RW_BYTE_BUFFER_BINDING, DescriptorType::RW_STRUCTURED_BUFFER, nullptr, nullptr, &bufferInfo);
}

void ResourceViewRegistry::DestroyHandle(TextureViewHandle handle) noexcept
{
    DestroyHandle(m_textureHandleManager, handle, TEXTURE_BINDING);
}

void ResourceViewRegistry::DestroyHandle(RWTextureViewHandle handle) noexcept
{
    DestroyHandle(m_rwTextureHandleManager, handle, RW_TEXTURE_BINDING);
}

void ResourceViewRegistry::DestroyHandle(TypedBufferViewHandle handle) noexcept
{
    DestroyHandle(m_typedBufferHandleManager, handle, TYPED_BUFFER_BINDING);
}

void ResourceViewRegistry::DestroyHandle(RWTypedBufferViewHandle handle) noexcept
{
    DestroyHandle(m_rwTypedBufferHandleManager, handle, RW_TYPED_BUFFER_BINDING);
}

void ResourceViewRegistry::DestroyHandle(ByteBufferViewHandle handle) noexcept
{
    DestroyHandle(m_byteBufferHandleManager, handle, BYTE_BUFFER_BINDING);
}

void ResourceViewRegistry::DestroyHandle(RWByteBufferViewHandle handle) noexcept
{
    DestroyHandle(m_rwByteBufferHandleManager, handle, RW_BYTE_BUFFER_BINDING);
}

void ResourceViewRegistry::DestroyHandle(StructuredBufferViewHandle handle) noexcept
{
    DestroyHandle((ByteBufferViewHandle) handle);
}

void ResourceViewRegistry::DestroyHandle(RWStructuredBufferViewHandle handle) noexcept
{
    DestroyHandle((RWByteBufferViewHandle) handle);
}

void ResourceViewRegistry::FlushChanges() noexcept
{
    const size_t resIndex = m_frame % 2;

    SpinLockHolder spinLock(m_pendingUpdatesMutex[resIndex]);

    m_descriptorSets[resIndex]->Update((uint32_t) m_pendingUpdates[resIndex].size(), m_pendingUpdates[resIndex].data());
    m_pendingUpdates[resIndex].clear();
}

void ResourceViewRegistry::SwapSets() noexcept
{
    m_textureHandleManager.FreeTransientHandles();
    m_frame++;
}

DescriptorSetLayout* ResourceViewRegistry::GetDescriptorSetLayout() const noexcept
{
    return m_descriptorSetLayout;
}

DescriptorSet* ResourceViewRegistry::GetCurrentFrameDescriptorSet() const noexcept
{
    return m_descriptorSets[m_frame % 2];
}

void ResourceViewRegistry::AddUpdate(DescriptorSetUpdate& update, bool transient)
{
    for(size_t frame = 0; frame < 2; frame++)
    {
	if(transient && (frame != (m_frame % 2)))
	{
	    continue;
	}

	SpinLockHolder lockHolder(m_pendingUpdatesMutex[frame]);

	bool replacedExisting = false;

	for(auto& u: m_pendingUpdates[frame])
	{
	    if(u.DstBinding == update.DstBinding && u.DstArrayElement == update.DstArrayElement)
	    {
		u		 = update;
		replacedExisting = true;
	    }
	}

	if(!replacedExisting)
	{
	    m_pendingUpdates[frame].push_back(update);
	}
    }
}

uint32_t ResourceViewRegistry::CreateHandle(HandleManager& manager, SpinLock& managerMutex, uint32_t binding, bool transient, DescriptorType descriptorType, ImageView* imageView, BufferView* bufferView, const DescriptorBufferInfo* bufferInfo)
{
    uint32_t handle = 0;

    {
	SpinLockHolder lockHolder(managerMutex);
	handle = manager.Allocate(transient);
    }

    if(!handle)
    {
	return handle;
    }

    DescriptorSetUpdate update = {};
    {
	update.DescriptorType  = descriptorType;
	update.DstBinding      = binding;
	update.DstArrayElement = (uint32_t) handle;
	update.DescriptorCount = 1;
	update.ImageView       = imageView;
	update.BufferView      = bufferView;
    }

    if(bufferInfo)
    {
	update.BufferInfo1 = *bufferInfo;
    }

    AddUpdate(update, transient);

    return handle;
}

void ResourceViewRegistry::UpdateHandle(uint32_t handle, uint32_t binding, DescriptorType descriptorType, ImageView* imageView, BufferView* bufferView, const DescriptorBufferInfo* bufferInfo)
{
    DescriptorSetUpdate update = {};
    {
	update.DescriptorType  = descriptorType;
	update.DstBinding      = binding;
	update.DstArrayElement = (uint32_t) handle;
	update.DescriptorCount = 1;
	update.ImageView       = imageView;
	update.BufferView      = bufferView;
    }
    if(bufferInfo)
    {
	update.BufferInfo1 = *bufferInfo;
    }

    AddUpdate(update, false);
}

void ResourceViewRegistry::DestroyHandle(HandleManager& manager, uint32_t handle, uint32_t binding)
{
    for(size_t frame = 0; frame < 2; frame++)
    {
	SpinLockHolder lockHolder(m_pendingUpdatesMutex[frame]);

	auto& updates = m_pendingUpdates[frame];

	auto equals = [&](const auto& u)
	{
	    return u.DstBinding == binding && u.DstArrayElement == handle;
	};

	updates.erase(eastl::remove_if(updates.begin(), updates.end(), equals), updates.end());
    }
}
