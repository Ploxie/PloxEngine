//
// Created by Ploxie on 2023-05-24.
//

#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/vec3.hpp"

class DescriptorSet;
class Buffer;

struct CameraRenderViewData
{
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewProjectionMatrix;
    glm::vec3 CameraPosition;
};

struct RenderViewData
{
    CameraRenderViewData CameraData;
    DescriptorSet* OffsetBufferSet;
    Buffer* ConstantBuffer;
};
