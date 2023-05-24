//
// Created by Ploxie on 2023-05-23.
//

#pragma once
#include "EASTL/vector.h"
#include "rendergraph/ViewHandles.h"
#include "rendering/types/DescriptorSet.h"
#include "utility/HandleManager.h"
#include "utility/SpinLock.h"
#include <cstdint>

class GraphicsAdapter;
class ImageView;

class ResourceViewRegistry
{
public:
    static constexpr uint32_t TEXTURE_BINDING	      = 0;
    static constexpr uint32_t RW_TEXTURE_BINDING      = 1;
    static constexpr uint32_t TYPED_BUFFER_BINDING    = 2;
    static constexpr uint32_t RW_TYPED_BUFFER_BINDING = 3;
    static constexpr uint32_t BYTE_BUFFER_BINDING     = 4;
    static constexpr uint32_t RW_BYTE_BUFFER_BINDING  = 5;

    explicit ResourceViewRegistry(GraphicsAdapter* adapter);
    ~ResourceViewRegistry();

    ResourceViewRegistry(const ResourceViewRegistry&)		  = delete;
    ResourceViewRegistry(const ResourceViewRegistry&&)		  = delete;
    ResourceViewRegistry& operator=(const ResourceViewRegistry&)  = delete;
    ResourceViewRegistry& operator=(const ResourceViewRegistry&&) = delete;

    TextureViewHandle CreateTextureViewHandle(ImageView* imageView, bool transient = false) noexcept;
    RWTextureViewHandle CreateRWTextureViewHandle(ImageView* imageView, bool transient = false) noexcept;
    TypedBufferViewHandle CreateTypedBufferViewHandle(BufferView* bufferView, bool transient = false) noexcept;
    RWTypedBufferViewHandle CreateRWTypedBufferViewHandle(BufferView* bufferView, bool transient = false) noexcept;
    ByteBufferViewHandle CreateByteBufferViewHandle(const DescriptorBufferInfo& bufferInfo, bool transient = false) noexcept;
    RWByteBufferViewHandle CreateRWByteBufferViewHandle(const DescriptorBufferInfo& bufferInfo, bool transient = false) noexcept;
    StructuredBufferViewHandle CreateStructuredBufferViewHandle(const DescriptorBufferInfo& bufferInfo, bool transient = false) noexcept;
    RWStructuredBufferViewHandle CreateRWStructuredBufferViewHandle(const DescriptorBufferInfo& bufferInfo, bool transient = false) noexcept;

    void UpdateHandle(TextureViewHandle handle, ImageView* imageView) noexcept;
    void UpdateHandle(RWTextureViewHandle handle, ImageView* imageView) noexcept;
    void UpdateHandle(TypedBufferViewHandle handle, BufferView* bufferView) noexcept;
    void UpdateHandle(RWTypedBufferViewHandle handle, BufferView* bufferView) noexcept;
    void UpdateHandle(ByteBufferViewHandle handle, const DescriptorBufferInfo& bufferInfo) noexcept;
    void UpdateHandle(RWByteBufferViewHandle handle, const DescriptorBufferInfo& bufferInfo) noexcept;
    void UpdateHandle(StructuredBufferViewHandle handle, const DescriptorBufferInfo& bufferInfo) noexcept;
    void UpdateHandle(RWStructuredBufferViewHandle handle, const DescriptorBufferInfo& bufferInfo) noexcept;

    void DestroyHandle(TextureViewHandle handle) noexcept;
    void DestroyHandle(RWTextureViewHandle handle) noexcept;
    void DestroyHandle(TypedBufferViewHandle handle) noexcept;
    void DestroyHandle(RWTypedBufferViewHandle handle) noexcept;
    void DestroyHandle(ByteBufferViewHandle handle) noexcept;
    void DestroyHandle(RWByteBufferViewHandle handle) noexcept;
    void DestroyHandle(StructuredBufferViewHandle handle) noexcept;
    void DestroyHandle(RWStructuredBufferViewHandle handle) noexcept;

    void FlushChanges() noexcept;
    void SwapSets() noexcept;

    DescriptorSetLayout* GetDescriptorSetLayout() const noexcept;
    DescriptorSet* GetCurrentFrameDescriptorSet() const noexcept;

private:
    void AddUpdate(DescriptorSetUpdate& update, bool transient);
    uint32_t CreateHandle(HandleManager& manager, SpinLock& managerMutex, uint32_t binding, bool transient, DescriptorType descriptorType, ImageView* imageView, BufferView* bufferView, const DescriptorBufferInfo* bufferInfo);
    void UpdateHandle(uint32_t handle, uint32_t binding, DescriptorType descriptorType, ImageView* imageView, BufferView* bufferView, const DescriptorBufferInfo* bufferInfo);
    void DestroyHandle(HandleManager& manager, uint32_t handle, uint32_t binding);

private:
    GraphicsAdapter* m_adapter		       = nullptr;
    DescriptorSetPool* m_descriptorSetPool     = nullptr;
    DescriptorSetLayout* m_descriptorSetLayout = nullptr;
    DescriptorSet* m_descriptorSets[2]	       = {};
    eastl::vector<DescriptorSetUpdate> m_pendingUpdates[2];
    uint32_t m_frame = 0;

    HandleManager m_textureHandleManager;
    HandleManager m_rwTextureHandleManager;
    HandleManager m_typedBufferHandleManager;
    HandleManager m_rwTypedBufferHandleManager;
    HandleManager m_byteBufferHandleManager;
    HandleManager m_rwByteBufferHandleManager;

    SpinLock m_pendingUpdatesMutex[2];
    SpinLock m_textureHandleManagerMutex;
    SpinLock m_rwTextureHandleManagerMutex;
    SpinLock m_typedBufferHandleManagerMutex;
    SpinLock m_rwTypedBufferHandleManagerMutex;
    SpinLock m_byteBufferHandleManagerMutex;
    SpinLock m_rwByteBufferHandleManagerMutex;
};