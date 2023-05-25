//
// Created by Ploxie on 2023-05-24.
//

#pragma once
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "rendering/pass/CubePass.h"
#include "rendering/rendergraph/RenderGraph.h"
#include "rendering/rendergraph/ViewHandles.h"
#include "RenderViewData.h"
#include <cstdint>

class DescriptorSetLayout;

class RenderView
{
public:
    struct Data
    {
	CameraRenderViewData CameraData;
	DescriptorSet* OffsetBufferSet;
	Buffer* ConstantBuffer;
	uint32_t FrameWidth;
	uint32_t FrameHeight;
    };

public:
    explicit RenderView(GraphicsAdapter* adapter, ResourceViewRegistry* viewRegistry, DescriptorSetLayout* offsetBufferSetLayout, uint32_t width, uint32_t height) noexcept;

    void Render(const Data& data, RenderGraph* renderGraph) noexcept;

    Image* GetResultImage() const noexcept;
    ImageView* GetResultImageView();
    ResourceViewHandle GetResultImageViewHandle() const noexcept;

private:
    void CreateImageResources(uint32_t width, uint32_t height) noexcept;

private:
    GraphicsAdapter* m_adapter;
    ResourceViewRegistry* m_resourceRegistry;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_frame;

    RenderViewData m_viewData[2] = {};

    CubePass* m_cubePass;

    ResourceViewHandle m_resultImageViewHandle;
    ResourceViewHandle m_resultDepthViewHandle;

    struct Resources
    {
	Image* ResultImage			       = nullptr;
	ImageView* ResultImageView		       = nullptr;
	TextureViewHandle ResultImageTextureViewHandle = {};
	ResourceStateData ResultImageState[1]	       = {};
    } m_resources;
};