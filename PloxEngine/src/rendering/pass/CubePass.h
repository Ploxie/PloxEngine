//
// Created by Ploxie on 2023-05-24.
//

#pragma once

#include "rendering/rendergraph/ViewHandles.h"
#include "rendering/renderview/RenderViewData.h"

class GraphicsAdapter;
class RenderGraph;
class GraphicsPipeline;
class DescriptorSetLayout;
class DescriptorSet;
class Buffer;

class CubePass
{
public:
    struct Data
    {
	glm::mat4 ModelMatrix;
	CameraRenderViewData CameraData;
	ResourceViewHandle ColorAttachment;
	ResourceViewHandle DepthAttachment;
	DescriptorSet* OffsetBufferSet;
	Buffer* ConstantBuffer;
	uint32_t FrameWidth;
	uint32_t FrameHeight;
    };

public:
    explicit CubePass(GraphicsAdapter* adapter, DescriptorSetLayout* offsetBufferSetLayout);
    ~CubePass();

    void Record(RenderGraph* renderGraph, const Data& data);

private:
    GraphicsAdapter* m_adapter;
    GraphicsPipeline* m_pipeline;

    Buffer* m_indexBuffer;
    Buffer* m_vertexBuffer;
    uint16_t m_indices[36];

    float m_rotation = 0.0f;

    struct Vertex
    {
	glm::vec3 Position;
	glm::vec4 Color;
    } m_vertices[8];
};
