//
// Created by Ploxie on 2023-05-24.
//

#include "CubePass.h"
#include "rendering/GraphicsAdapter.h"
#include "rendering/rendergraph/Registry.h"
#include "rendering/rendergraph/RenderGraph.h"
#include "rendering/RenderUtilities.h"
#include "rendering/types/Command.h"
#include "rendering/types/GraphicsPipeline.h"
#include <glm/gtc/quaternion.hpp>

CubePass::CubePass(GraphicsAdapter* adapter, DescriptorSetLayout* offsetBufferSetLayout)
    : m_adapter(adapter)
{
    PipelineColorBlendAttachmentState blendState = {};
    {
	blendState.BlendEnable	       = false;
	blendState.SrcColorBlendFactor = BlendFactor::ZERO;
	blendState.DstColorBlendFactor = BlendFactor::ZERO;
	blendState.ColorBlendOp	       = BlendOp::ADD;
	blendState.SrcAlphaBlendFactor = BlendFactor::ZERO;
	blendState.DstAlphaBlendFactor = BlendFactor::ZERO;
	blendState.AlphaBlendOp	       = BlendOp::ADD;
	blendState.ColorWriteMask      = ColorComponentFlags::ALL_BITS;
    }

    VertexInputAttributeDescription attributeDescription[] = {
	{ "POSITION", 0, 0, Format::R32G32B32_SFLOAT, 0 },
	{ "NORMAL", 1, 1, Format::R32G32B32_SFLOAT, 0 }
    };

    VertexInputBindingDescription bindingDescriptions[] = {
	{ 0, sizeof(float) * 3, VertexInputRate::VERTEX },
	{ 1, sizeof(float) * 3, VertexInputRate::VERTEX },
    };

    GraphicsPipelineCreateInfo pipelineCreateInfo = {};
    GraphicsPipelineBuilder builder(pipelineCreateInfo);
    builder.SetVertexShader("assets/shaders/cube_vs");
    builder.SetFragmentShader("assets/shaders/cube_ps");
    builder.SetVertexBindingDescriptions(eastl::size(bindingDescriptions), bindingDescriptions);
    builder.SetVertexAttributeDescriptions((uint32_t) eastl::size(attributeDescription), attributeDescription);
    builder.SetColorBlendAttachment(blendState);
    builder.SetDepthTest(true, false, CompareOp::GREATER_OR_EQUAL);
    builder.SetDynamicState(DynamicStateFlags::VIEWPORT_BIT | DynamicStateFlags::SCISSOR_BIT);
    builder.SetDepthStencilAttachmentFormat(Format::D32_SFLOAT);
    builder.SetColorAttachmentFormat(Format::B8G8R8A8_UNORM);
    builder.SetPolygonModeCullMode(PolygonMode::FILL, CullModeFlags::BACK_BIT, FrontFace::COUNTER_CLOCKWISE);

    DescriptorSetLayoutBinding usedBinding { DescriptorType::OFFSET_CONSTANT_BUFFER, 0, 0, 1, ShaderStageFlags::ALL_STAGES };
    DescriptorSetLayoutDeclaration layoutDeclaration { offsetBufferSetLayout, 1, &usedBinding };
    builder.SetPipelineLayoutDescription(1, &layoutDeclaration, 0, {}, 0, nullptr, 0);

    m_adapter->CreateGraphicsPipeline(1, &pipelineCreateInfo, &m_pipeline);

    uint32_t indices[] = {
	0, 1, 3, 1, 2, 3,
	4, 5, 7, 5, 6, 7,
	8, 9, 11, 9, 10, 11,
	12, 13, 15, 13, 14, 15,
	16, 17, 19, 17, 18, 19,
	20, 21, 23, 21, 22, 23
    };

    BufferCreateInfo indexBufferCreateInfo = {};
    {
	indexBufferCreateInfo.Size	 = eastl::size(indices) * sizeof(uint32_t);
	indexBufferCreateInfo.UsageFlags = BufferUsageFlags::INDEX_BUFFER_BIT | BufferUsageFlags::TRANSFER_DST_BIT;
    }
    m_adapter->CreateBuffer(indexBufferCreateInfo, MemoryPropertyFlags::HOST_VISIBLE_BIT | MemoryPropertyFlags::HOST_COHERENT_BIT, MemoryPropertyFlags::DEVICE_LOCAL_BIT, false, &m_indexBuffer);
    m_adapter->SetDebugObjectName(ObjectType::BUFFER, m_indexBuffer, "Index Buffer");

    uint32_t* indicesPtr;
    m_indexBuffer->Map((void**) &indicesPtr);
    memcpy(indicesPtr, indices, indexBufferCreateInfo.Size);

    BufferCreateInfo vertexBufferCreateInfo = {};
    {
	vertexBufferCreateInfo.Size	  = 6 * sizeof(float) * 24;
	vertexBufferCreateInfo.UsageFlags = BufferUsageFlags::VERTEX_BUFFER_BIT | BufferUsageFlags::TRANSFER_DST_BIT;
    }
    m_adapter->CreateBuffer(vertexBufferCreateInfo, MemoryPropertyFlags::HOST_COHERENT_BIT, MemoryPropertyFlags::DEVICE_LOCAL_BIT, false, &m_vertexBuffer);
    m_adapter->SetDebugObjectName(ObjectType::BUFFER, m_vertexBuffer, "Vertex Buffer");

    glm::vec3 positions[48] = {
	{ -0.5f, -0.5f, 0.5f }, // Front
	{ 0.5f, -0.5f, 0.5f },
	{ 0.5f, 0.5f, 0.5f },
	{ -0.5f, 0.5f, 0.5f },

	{ 0.5f, -0.5f, -0.5f }, // Back
	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, 0.5f, -0.5f },
	{ 0.5f, 0.5f, -0.5f },

	{ -0.5f, -0.5f, -0.5f }, // Up
	{ 0.5f, -0.5f, -0.5f },
	{ 0.5f, -0.5f, 0.5f },
	{ -0.5f, -0.5f, 0.5f },

	{ -0.5f, 0.5f, 0.5f }, // Down
	{ 0.5f, 0.5f, 0.5f },
	{ 0.5f, 0.5f, -0.5f },
	{ -0.5f, 0.5f, -0.5f },

	{ -0.5f, 0.5f, 0.5f }, // Left
	{ -0.5f, 0.5f, -0.5f },
	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f, 0.5f },

	{ 0.5f, 0.5f, -0.5f }, // Right
	{ 0.5f, 0.5f, 0.5f },
	{ 0.5f, -0.5f, 0.5f },
	{ 0.5f, -0.5f, -0.5f },

	{ 0, 0, 1 }, // Front
	{ 0, 0, 1 },
	{ 0, 0, 1 },
	{ 0, 0, 1 },

	{ 0, 0, -1 }, // Back
	{ 0, 0, -1 },
	{ 0, 0, -1 },
	{ 0, 0, -1 },

	{ 0, -1, 0 }, // Down
	{ 0, -1, 0 },
	{ 0, -1, 0 },
	{ 0, -1, 0 },

	{ 0, 1, 0 }, // Up
	{ 0, 1, 0 },
	{ 0, 1, 0 },
	{ 0, 1, 0 },

	{ -1, 0, 0 }, // Left
	{ -1, 0, 0 },
	{ -1, 0, 0 },
	{ -1, 0, 0 },

	{ 1, 0, 0 }, // Right
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 },
    };

    float vertices[] = {
	// Vertices
	-0.5f,
	0.5f,
	-0.5f,

	0.5f,
	0.5f,
	-0.5f,

	0.5f,
	0.5f,
	0.5f,

	-0.5f,
	0.5f,
	0.5f,

	-0.5f,
	-0.5f,
	-0.5f,

	0.5f,
	-0.5f,
	-0.5f,

	0.5f,
	-0.5f,
	0.5f,

	-0.5f,
	-0.5f,
	0.5f,

	// Normals
	0.0f,
	0.0f,
	1.0f,

	0.0f,
	0.0f,
	1.0f,

	0.0f,
	0.0f,
	1.0f,

	-1.0f,
	0.0f,
	0.0f,

	0.0f,
	1.0f,
	0.0f,

	0.0f,
	-1.0f,
	0.0f,

	0.0f,
	-1.0f,
	0.0f,

	0.0f,
	-1.0f,
	0.0f,
    };

    float* verticesPtr;
    m_vertexBuffer->Map((void**) &verticesPtr);
    size_t size = 24 * sizeof(float) * 6;

    memcpy(verticesPtr, positions, size);
    //memcpy(verticesPtr + size, normals, size);
}

CubePass::~CubePass()
{
    //m_adapter->DestroyGraphicsPipeline(m_pipeline);
}

void CubePass::Record(RenderGraph* renderGraph, const CubePass::Data& data)
{
    m_rotation += 0.0005f;

    ResourceUsageDescription usageDescs[] = {
	{ data.ColorAttachment, { ResourceState::WRITE_COLOR_ATTACHMENT } },
	{ data.DepthAttachment, { ResourceState::WRITE_DEPTH_STENCIL } },

    };

    renderGraph->AddPass("Cube", QueueType::GRAPHICS, eastl::size(usageDescs), usageDescs, [=](Command* command, const Registry& registry)
    {
	struct Constants
	{
	    float modelMatrix[16];
	    float viewMatrix[16];
	    float projectionMatrix[16];
	    float cameraPosition[3];
	    float padding;
	};

	Constants consts = {};

	glm::quat tmp	= glm::quat(glm::angleAxis(0.785398163f, glm::vec3(0.0f, 0.0f, 1.0f)));
	glm::quat tmp2	= glm::quat(glm::angleAxis(0.785398163f, glm::vec3(1.0f, 0.0f, .0f)));
	glm::quat tmp3	= glm::quat(glm::angleAxis(m_rotation, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::quat rot	= glm::normalize(tmp3 * tmp2 * tmp);
	glm::mat4 model = glm::mat4_cast(rot);

	memcpy(consts.modelMatrix, &model[0], 16 * sizeof(float));
	memcpy(consts.viewMatrix, &data.CameraData.ViewMatrix, sizeof(consts.viewMatrix));
	memcpy(consts.projectionMatrix, &data.CameraData.ProjectionMatrix, sizeof(consts.projectionMatrix));
	memcpy(consts.cameraPosition, &data.CameraData.CameraPosition, sizeof(consts.cameraPosition));

	Constants* cb;
	data.ConstantBuffer->Map((void**) &cb);
	memcpy(cb, &consts, sizeof(Constants));

	//uint32_t allocOffset = (uint32_t) data.m_bufferAllocator->uploadStruct(DescriptorType::OFFSET_CONSTANT_BUFFER, consts);

	ColorAttachmentDescription attachmentDesc { registry.GetImageView(data.ColorAttachment), AttachmentLoadOp::CLEAR, AttachmentStoreOp::STORE, { 0.01f, 0.02f, 0.02f, 1.0f } };
	DepthStencilAttachmentDescription depthBufferDesc { registry.GetImageView(data.DepthAttachment), AttachmentLoadOp::CLEAR, AttachmentStoreOp::STORE, AttachmentLoadOp::DONT_CARE, AttachmentStoreOp::DONT_CARE };
	Rect renderRect { { 0, 0 }, { data.FrameWidth, data.FrameHeight } };

	command->BeginRenderPass(1, &attachmentDesc, &depthBufferDesc, renderRect, false);
	{
	    command->BindPipeline(m_pipeline);

	    Viewport viewport { 0.0f, 0.0f, (float) data.FrameWidth, (float) data.FrameHeight, 0.0f, 1.0f };
	    command->SetViewports(0, 1, &viewport);
	    Rect scissor { { 0, 0 }, { data.FrameWidth, data.FrameHeight } };
	    command->SetScissors(0, 1, &scissor);

	    Buffer* vertexBuffers[] = { m_vertexBuffer, m_vertexBuffer };

	    uint32_t offset = 0; //36 * sizeof(float);
	    command->BindDescriptorSets(m_pipeline, 0, 1, &data.OffsetBufferSet, 1, &offset);

	    command->BindIndexBuffer(m_indexBuffer, 0, IndexType::UINT32);
	    uint64_t offsets[2] = { 0, 288 };
	    command->BindVertexBuffers(0, 2, vertexBuffers, offsets);
	    command->DrawIndexed(36, 1, 0, 0, 0);
	}
	command->EndRenderPass();
    });
}
