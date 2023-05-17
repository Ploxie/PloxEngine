//
// Created by Ploxie on 2023-05-17.
//

#pragma once
#include "Extent2D.h"
#include "Format.h"
#include "utility/Enum.h"
#include <cstdint>

class DescriptorSetLayout;

struct ShaderStageCreateInfo
{
    enum
    {
	MAX_PATH_LENGTH = 255
    };
    char Path[MAX_PATH_LENGTH + 1] = {};
};

enum class VertexInputRate
{
    VERTEX,
    INSTANCE
};

struct VertexInputBindingDescription
{
    uint32_t Binding;
    uint32_t Stride;
    VertexInputRate InputRate;
};

struct VertexInputAttributeDescription
{
    enum
    {
	MAX_SEMANTIC_NAME_LENGTH = 63
    };
    char SemanticName[MAX_SEMANTIC_NAME_LENGTH + 1] = {};
    uint32_t Location;
    uint32_t Binding;
    Format Format;
    uint32_t Offset;
};

enum class PrimitiveTopology
{
    POINT_LIST			  = 0,
    LINE_LIST			  = 1,
    LINE_STRIP			  = 2,
    TRIANGLE_LIST		  = 3,
    TRIANGLE_STRIP		  = 4,
    TRIANGLE_FAN		  = 5,
    LINE_LIST_WITH_ADJACENCY	  = 6,
    LINE_STRIP_WITH_ADJACENCY	  = 7,
    TRIANGLE_LIST_WITH_ADJACENCY  = 8,
    TRIANGLE_STRIP_WITH_ADJACENCY = 9,
    PATCH_LIST			  = 10,
};

struct Viewport
{
    float X;
    float Y;
    float Width;
    float Height;
    float MinDepth;
    float MaxDepth;
};

struct Rect
{
    Offset2D Offset;
    Extent2D Extent;
};

enum class PolygonMode
{
    FILL  = 0,
    LINE  = 1,
    POINT = 2,
};

enum class CullModeFlags
{
    NONE	   = 0,
    FRONT_BIT	   = 1,
    BACK_BIT	   = 2,
    FRONT_AND_BACK = FRONT_BIT | BACK_BIT,
};
DEF_ENUM_FLAG_OPERATORS(CullModeFlags);

enum class ColorComponentFlags
{
    R_BIT    = 0x00000001,
    G_BIT    = 0x00000002,
    B_BIT    = 0x00000004,
    A_BIT    = 0x00000008,
    ALL_BITS = R_BIT | G_BIT | B_BIT | A_BIT,
};
DEF_ENUM_FLAG_OPERATORS(ColorComponentFlags);

enum class DynamicStateFlags
{
    VIEWPORT_BIT	     = 1u << 0u,
    SCISSOR_BIT		     = 1u << 1u,
    LINE_WIDTH_BIT	     = 1u << 2u,
    DEPTH_BIAS_BIT	     = 1u << 3u,
    BLEND_CONSTANTS_BIT	     = 1u << 4u,
    DEPTH_BOUNDS_BIT	     = 1u << 5u,
    STENCIL_COMPARE_MASK_BIT = 1u << 6u,
    STENCIL_WRITE_MASK_BIT   = 1u << 7u,
    STENCIL_REFERENCE_BIT    = 1u << 8u,
};
DEF_ENUM_FLAG_OPERATORS(DynamicStateFlags);

enum class ShaderStageFlags
{
    VERTEX_BIT	 = 0x00000001,
    HULL_BIT	 = 0x00000002,
    DOMAIN_BIT	 = 0x00000004,
    GEOMETRY_BIT = 0x00000008,
    PIXEL_BIT	 = 0x00000010,
    COMPUTE_BIT	 = 0x00000020,
    ALL_STAGES	 = VERTEX_BIT | HULL_BIT | DOMAIN_BIT | GEOMETRY_BIT | PIXEL_BIT | COMPUTE_BIT,
};
DEF_ENUM_FLAG_OPERATORS(ShaderStageFlags);

enum class DescriptorBindingFlags
{
    UPDATE_AFTER_BIND_BIT	    = 1u << 0u,
    UPDATE_UNUSED_WHILE_PENDING_BIT = 1u << 1u,
    PARTIALLY_BOUND_BIT		    = 1u << 2u,
};
DEF_ENUM_FLAG_OPERATORS(DescriptorBindingFlags);

enum class FrontFace
{
    COUNTER_CLOCKWISE = 0,
    CLOCKWISE	      = 1,
};

enum class SampleCount
{
    _1	= 0x00000001,
    _2	= 0x00000002,
    _4	= 0x00000004,
    _8	= 0x00000008,
    _16 = 0x00000010,
};

enum class CompareOp
{
    NEVER	     = 0,
    LESS	     = 1,
    EQUAL	     = 2,
    LESS_OR_EQUAL    = 3,
    GREATER	     = 4,
    NOT_EQUAL	     = 5,
    GREATER_OR_EQUAL = 6,
    ALWAYS	     = 7,
};

enum class StencilOp
{
    KEEP		= 0,
    ZERO		= 1,
    REPLACE		= 2,
    INCREMENT_AND_CLAMP = 3,
    DECREMENT_AND_CLAMP = 4,
    INVERT		= 5,
    INCREMENT_AND_WRAP	= 6,
    DECREMENT_AND_WRAP	= 7,
};

struct StencilOpState
{
    StencilOp FailOp;
    StencilOp PassOp;
    StencilOp DepthFailOp;
    CompareOp CompareOp;
    uint32_t CompareMask;
    uint32_t WriteMask;
    uint32_t Reference;
};

enum class LogicOp
{
    CLEAR	  = 0,
    AND		  = 1,
    AND_REVERSE	  = 2,
    COPY	  = 3,
    AND_INVERTED  = 4,
    NO_OP	  = 5,
    XOR		  = 6,
    OR		  = 7,
    NOR		  = 8,
    EQUIVALENT	  = 9,
    INVERT	  = 10,
    OR_REVERSE	  = 11,
    COPY_INVERTED = 12,
    OR_INVERTED	  = 13,
    NAND	  = 14,
    SET		  = 15,
};

enum class BlendFactor
{
    ZERO		     = 0,
    ONE			     = 1,
    SRC_COLOR		     = 2,
    ONE_MINUS_SRC_COLOR	     = 3,
    DST_COLOR		     = 4,
    ONE_MINUS_DST_COLOR	     = 5,
    SRC_ALPHA		     = 6,
    ONE_MINUS_SRC_ALPHA	     = 7,
    DST_ALPHA		     = 8,
    ONE_MINUS_DST_ALPHA	     = 9,
    CONSTANT_COLOR	     = 10,
    ONE_MINUS_CONSTANT_COLOR = 11,
    CONSTANT_ALPHA	     = 12,
    ONE_MINUS_CONSTANT_ALPHA = 13,
    SRC_ALPHA_SATURATE	     = 14,
    SRC1_COLOR		     = 15,
    ONE_MINUS_SRC1_COLOR     = 16,
    SRC1_ALPHA		     = 17,
    ONE_MINUS_SRC1_ALPHA     = 18,
};

enum class BlendOp
{
    ADD		     = 0,
    SUBTRACT	     = 1,
    REVERSE_SUBTRACT = 2,
    MIN		     = 3,
    MAX		     = 4,
};

struct PipelineColorBlendAttachmentState
{
    bool BlendEnable;
    BlendFactor SrcColorBlendFactor;
    BlendFactor DstColorBlendFactor;
    BlendOp ColorBlendOp;
    BlendFactor SrcAlphaBlendFactor;
    BlendFactor DstAlphaBlendFactor;
    BlendOp AlphaBlendOp;
    ColorComponentFlags ColorWriteMask;
};

struct AttachmentFormats
{
    uint32_t ColorAttachmentCount    = 0;
    Format ColorAttachmentFormats[8] = {};
    Format DepthStencilFormat	     = Format::UNDEFINED;
};

enum class DescriptorType
{
    SAMPLER		   = 0,
    TEXTURE		   = 1,
    RW_TEXTURE		   = 2,
    TYPED_BUFFER	   = 3,
    RW_TYPED_BUFFER	   = 4,
    CONSTANT_BUFFER	   = 5,
    BYTE_BUFFER		   = 6,
    RW_BYTE_BUFFER	   = 7,
    STRUCTURED_BUFFER	   = 8,
    RW_STRUCTURED_BUFFER   = 9,
    OFFSET_CONSTANT_BUFFER = 10,
    RANGE_SIZE		   = OFFSET_CONSTANT_BUFFER + 1
};

enum class Filter
{
    NEAREST = 0,
    LINEAR  = 1,
};

enum class SamplerMipmapMode
{
    NEAREST = 0,
    LINEAR  = 1,
};

enum class SamplerAddressMode
{
    REPEAT		 = 0,
    MIRRORED_REPEAT	 = 1,
    CLAMP_TO_EDGE	 = 2,
    CLAMP_TO_BORDER	 = 3,
    MIRROR_CLAMP_TO_EDGE = 4,
};

enum class BorderColor
{
    FLOAT_TRANSPARENT_BLACK = 0,
    INT_TRANSPARENT_BLACK   = 1,
    FLOAT_OPAQUE_BLACK	    = 2,
    INT_OPAQUE_BLACK	    = 3,
    FLOAT_OPAQUE_WHITE	    = 4,
    INT_OPAQUE_WHITE	    = 5,
};

struct StaticSamplerDescription
{
    uint32_t Binding;
    uint32_t Space;
    ShaderStageFlags StageFlags;
    Filter MagFilter;
    Filter MinFilter;
    SamplerMipmapMode MipmapMode;
    SamplerAddressMode AddressModeU;
    SamplerAddressMode AddressModeV;
    SamplerAddressMode AddressModeW;
    float MipLodBias;
    bool AnisotropyEnable;
    float MaxAnisotropy;
    bool CompareEnable;
    CompareOp CompareOp;
    float MinLod;
    float MaxLod;
    BorderColor BorderColor;
    bool UnnormalizedCoordinates;
};

struct DescriptorSetLayoutBinding
{
    DescriptorType DescriptorType;
    uint32_t Binding;
    uint32_t Space;
    uint32_t DescriptorCount;
    ShaderStageFlags StageFlags;
    DescriptorBindingFlags BindingFlags;
};

struct DescriptorSetLayoutDeclaration
{
    DescriptorSetLayout* Layout;
    uint32_t UsedBindingCount;
    DescriptorSetLayoutBinding* UsedBindings;
};

struct PipelineLayoutCreateInfo
{
    uint32_t DescriptorSetLayoutCount;
    DescriptorSetLayoutDeclaration DescriptorSetLayoutDeclarations[4];
    uint32_t PushConstRange;
    ShaderStageFlags PushConstStageFlags;
    uint32_t StaticSamplerSet;
    uint32_t StaticSamplerCount;
    const StaticSamplerDescription* StaticSamplerDescriptions;
};

struct VertexInputState
{
    enum
    {
	MAX_VERTEX_BINDING_DESCRIPTIONS	  = 8,
	MAX_VERTEX_ATTRIBUTE_DESCRIPTIONS = 8,
    };
    uint32_t VertexBindingDescriptionCount							   = 0;
    VertexInputBindingDescription VertexBindingDescriptions[MAX_VERTEX_BINDING_DESCRIPTIONS]	   = {};
    uint32_t VertexAttributeDescriptionCount							   = 0;
    VertexInputAttributeDescription VertexAttributeDescriptions[MAX_VERTEX_ATTRIBUTE_DESCRIPTIONS] = {};
};

struct InputAssemblyState
{
    PrimitiveTopology PrimitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
    uint32_t PrimitiveRestartEnable	= false;
};

struct TesselationState
{
    uint32_t PatchControlPoints = 0;
};

struct ViewportState
{
    enum
    {
	MAX_VIEWPORTS = 1
    };
    uint32_t ViewportCount	      = 1;
    Viewport Viewports[MAX_VIEWPORTS] = { { 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f } };
    Rect Scissors[MAX_VIEWPORTS]      = { { { 0, 0 }, { 1, 1 } } };
};

struct RasterizationState
{
    uint32_t DepthClampEnable	     = false;
    uint32_t RasterizerDiscardEnable = false;
    PolygonMode PolygonMode	     = PolygonMode::FILL;
    CullModeFlags CullMode	     = CullModeFlags::NONE;
    FrontFace FrontFace		     = FrontFace::COUNTER_CLOCKWISE;
    uint32_t DepthBiasEnable	     = false;
    float DepthBiasConstantFactor    = 1.0f;
    float DepthBiasClamp	     = 0.0f;
    float DepthBiasSlopeFactor	     = 1.0f;
    float LineWidth		     = 1.0f;
};

struct MultisampleState
{
    SampleCount RasterizationSamples = SampleCount::_1;
    uint32_t SampleShadingEnable     = false;
    float MinSampleShading	     = 0.0f;
    uint32_t SampleMask		     = 0xFFFFFFFF;
    uint32_t AlphaToCoverageEnable   = false;
    uint32_t AlphaToOneEnable	     = false;
};

struct DepthStencilState
{
    uint32_t DepthTestEnable	   = false;
    uint32_t DepthWriteEnable	   = false;
    CompareOp DepthCompareOp	   = CompareOp::ALWAYS;
    uint32_t DepthBoundsTestEnable = false;
    uint32_t StencilTestEnable	   = false;
    StencilOpState Front	   = {};
    StencilOpState Back		   = {};
    float MinDepthBounds	   = 0.0f;
    float MaxDepthBounds	   = 1.0f;
};

struct BlendState
{
    uint32_t LogicOpEnable			     = false;
    LogicOp LogicOp				     = LogicOp::COPY;
    uint32_t AttachmentCount			     = 0;
    PipelineColorBlendAttachmentState Attachments[8] = {};
    float BlendConstants[4]			     = {};
};

struct GraphicsPipelineCreateInfo
{
    ShaderStageCreateInfo VertexShader;
    ShaderStageCreateInfo HullShader;
    ShaderStageCreateInfo DomainShader;
    ShaderStageCreateInfo GeometryShader;
    ShaderStageCreateInfo PixelShader;

    VertexInputState VertexInputState;
    InputAssemblyState InputAssemblyState;
    TesselationState TesselationState;
    ViewportState ViewportState;
    RasterizationState RasterizationState;
    MultisampleState MultiSampleState;
    DepthStencilState DepthStencilState;
    BlendState BlendState;
    DynamicStateFlags DynamicStateFlags = (DynamicStateFlags) 0;
    AttachmentFormats AttachmentFormats;
    PipelineLayoutCreateInfo LayoutCreateInfo;
};

class GraphicsPipeline
{
public:
    virtual ~GraphicsPipeline()							    = default;
    virtual void* GetNativeHandle() const					    = 0;
    virtual uint32_t GetDescriptorSetLayoutCount() const			    = 0;
    virtual const DescriptorSetLayout* GetDescriptorSetLayout(uint32_t index) const = 0;
};