//
// Created by Ploxie on 2023-05-11.
//
#include "VulkanUtilities.h"
#include "core/Assert.h"
#include "core/Logger.h"

VkResult VulkanUtilities::checkResult(VkResult result, const char *errorMsg, bool exitOnError)
{
    if(result == VK_SUCCESS)
    {
	return result;
    }

    constexpr size_t maxErrorMsgLength = 256;
    constexpr size_t maxResultLength   = 128;
    constexpr size_t msgBufferLength   = maxErrorMsgLength + maxResultLength;

    char msgBuffer[msgBufferLength];
    msgBuffer[0] = '\0';

    bool error = false;

    const char* defaultMessage = " ";
    if(!errorMsg)
    {
	errorMsg = defaultMessage;
    }

    switch(result)
    {
	case VK_SUCCESS:
	    break;
	case VK_NOT_READY:
	    break;
	case VK_TIMEOUT:
	    break;
	case VK_EVENT_SET:
	    break;
	case VK_EVENT_RESET:
	    break;
	case VK_INCOMPLETE:
	    break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
	    LOG_CORE_CRITICAL("VK_ERROR_OUT_OF_HOST_MEMORY: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
	    LOG_CORE_CRITICAL("VK_ERROR_OUT_OF_DEVICE_MEMORY: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_INITIALIZATION_FAILED:
	    LOG_CORE_CRITICAL("VK_ERROR_INITIALIZATION_FAILED: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_DEVICE_LOST:
	    LOG_CORE_CRITICAL("VK_ERROR_DEVICE_LOST: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_MEMORY_MAP_FAILED:
	    LOG_CORE_CRITICAL("VK_ERROR_MEMORY_MAP_FAILED: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_LAYER_NOT_PRESENT:
	    LOG_CORE_CRITICAL("VK_ERROR_LAYER_NOT_PRESENT: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
	    LOG_CORE_CRITICAL("VK_ERROR_EXTENSION_NOT_PRESENT: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_FEATURE_NOT_PRESENT:
	    LOG_CORE_CRITICAL("VK_ERROR_FEATURE_NOT_PRESENT: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
	    LOG_CORE_CRITICAL("VK_ERROR_INCOMPATIBLE_DRIVER: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_TOO_MANY_OBJECTS:
	    LOG_CORE_CRITICAL("VK_ERROR_TOO_MANY_OBJECTS: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
	    LOG_CORE_CRITICAL("VK_ERROR_FORMAT_NOT_SUPPORTED: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_FRAGMENTED_POOL:
	    LOG_CORE_CRITICAL("VK_ERROR_FRAGMENTED_POOL: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_UNKNOWN:
	    LOG_CORE_CRITICAL("VK_ERROR_UNKNOWN: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_OUT_OF_POOL_MEMORY:
	    LOG_CORE_CRITICAL("VK_ERROR_OUT_OF_POOL_MEMORY: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
	    LOG_CORE_CRITICAL("VK_ERROR_INVALID_EXTERNAL_HANDLE: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_FRAGMENTATION:
	    LOG_CORE_CRITICAL("VK_ERROR_FRAGMENTATION: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
	    LOG_CORE_CRITICAL("VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_SURFACE_LOST_KHR:
	    LOG_CORE_CRITICAL("VK_ERROR_SURFACE_LOST_KHR: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
	    LOG_CORE_CRITICAL("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: {0}", errorMsg);
	    error = true;
	    break;
	case VK_SUBOPTIMAL_KHR:
	    break;
	case VK_ERROR_OUT_OF_DATE_KHR:
	    LOG_CORE_CRITICAL("VK_ERROR_OUT_OF_DATE_KHR: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
	    LOG_CORE_CRITICAL("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_VALIDATION_FAILED_EXT:
	    LOG_CORE_CRITICAL("VK_ERROR_VALIDATION_FAILED_EXT: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_INVALID_SHADER_NV:
	    LOG_CORE_CRITICAL("VK_ERROR_INVALID_SHADER_NV: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
	    LOG_CORE_CRITICAL("VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_NOT_PERMITTED_EXT:
	    LOG_CORE_CRITICAL("VK_ERROR_NOT_PERMITTED_EXT: {0}", errorMsg);
	    error = true;
	    break;
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
	    LOG_CORE_CRITICAL("VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: {0}", errorMsg);
	    error = true;
	    break;
	default:
	    break;
    }

    ASSERT(!error);

    return result;
}

template<typename T>
static bool TestFlagBit(T flags, T bit)
{
    return (flags & bit) == bit;
}

void VulkanUtilities::Translate(DynamicStateFlags flags, uint32_t &stateCount, VkDynamicState states[9])
{
    if(TestFlagBit(flags, DynamicStateFlags::VIEWPORT_BIT))
    {
	states[stateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
    }
    if(TestFlagBit(flags, DynamicStateFlags::SCISSOR_BIT))
    {
	states[stateCount++] = VK_DYNAMIC_STATE_SCISSOR;
    }
    if(TestFlagBit(flags, DynamicStateFlags::LINE_WIDTH_BIT))
    {
	states[stateCount++] = VK_DYNAMIC_STATE_LINE_WIDTH;
    }
    if(TestFlagBit(flags, DynamicStateFlags::DEPTH_BIAS_BIT))
    {
	states[stateCount++] = VK_DYNAMIC_STATE_DEPTH_BIAS;
    }
    if(TestFlagBit(flags, DynamicStateFlags::BLEND_CONSTANTS_BIT))
    {
	states[stateCount++] = VK_DYNAMIC_STATE_BLEND_CONSTANTS;
    }
    if(TestFlagBit(flags, DynamicStateFlags::DEPTH_BOUNDS_BIT))
    {
	states[stateCount++] = VK_DYNAMIC_STATE_DEPTH_BOUNDS;
    }
    if(TestFlagBit(flags, DynamicStateFlags::STENCIL_COMPARE_MASK_BIT))
    {
	states[stateCount++] = VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
    }
    if(TestFlagBit(flags, DynamicStateFlags::STENCIL_WRITE_MASK_BIT))
    {
	states[stateCount++] = VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
    }
    if(TestFlagBit(flags, DynamicStateFlags::STENCIL_REFERENCE_BIT))
    {
	states[stateCount++] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;
    }
}

VkShaderStageFlags VulkanUtilities::Translate(ShaderStageFlags flags)
{
    VkShaderStageFlags result = 0;

    if(TestFlagBit(flags, ShaderStageFlags::VERTEX_BIT))
    {
	result |= VK_SHADER_STAGE_VERTEX_BIT;
    }
    if(TestFlagBit(flags, ShaderStageFlags::HULL_BIT))
    {
	result |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    }
    if(TestFlagBit(flags, ShaderStageFlags::DOMAIN_BIT))
    {
	result |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    }
    if(TestFlagBit(flags, ShaderStageFlags::GEOMETRY_BIT))
    {
	result |= VK_SHADER_STAGE_GEOMETRY_BIT;
    }
    if(TestFlagBit(flags, ShaderStageFlags::PIXEL_BIT))
    {
	result |= VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    if(TestFlagBit(flags, ShaderStageFlags::COMPUTE_BIT))
    {
	result |= VK_SHADER_STAGE_COMPUTE_BIT;
    }

    return result;
}

VkColorComponentFlags VulkanUtilities::Translate(ColorComponentFlags flags)
{
    VkColorComponentFlags result = 0;

    if(TestFlagBit(flags, ColorComponentFlags::R_BIT))
    {
	result |= VK_COLOR_COMPONENT_R_BIT;
    }
    if(TestFlagBit(flags, ColorComponentFlags::G_BIT))
    {
	result |= VK_COLOR_COMPONENT_G_BIT;
    }
    if(TestFlagBit(flags, ColorComponentFlags::B_BIT))
    {
	result |= VK_COLOR_COMPONENT_B_BIT;
    }
    if(TestFlagBit(flags, ColorComponentFlags::A_BIT))
    {
	result |= VK_COLOR_COMPONENT_A_BIT;
    }

    return result;
}

VkBlendOp VulkanUtilities::Translate(BlendOp blendOp)
{
    switch(blendOp)
    {
	case BlendOp::ADD:
	    return VK_BLEND_OP_ADD;
	case BlendOp::SUBTRACT:
	    return VK_BLEND_OP_SUBTRACT;
	case BlendOp::REVERSE_SUBTRACT:
	    return VK_BLEND_OP_REVERSE_SUBTRACT;
	case BlendOp::MIN:
	    return VK_BLEND_OP_MIN;
	case BlendOp::MAX:
	    return VK_BLEND_OP_MAX;
	default:
	    ASSERT(false);
	    break;
    }
    return VkBlendOp();
}

VkBlendFactor VulkanUtilities::Translate(BlendFactor blendFactor)
{
    switch(blendFactor)
    {
	case BlendFactor::ZERO:
	    return VK_BLEND_FACTOR_ZERO;
	case BlendFactor::ONE:
	    return VK_BLEND_FACTOR_ONE;
	case BlendFactor::SRC_COLOR:
	    return VK_BLEND_FACTOR_SRC_COLOR;
	case BlendFactor::ONE_MINUS_SRC_COLOR:
	    return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	case BlendFactor::DST_COLOR:
	    return VK_BLEND_FACTOR_DST_COLOR;
	case BlendFactor::ONE_MINUS_DST_COLOR:
	    return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	case BlendFactor::SRC_ALPHA:
	    return VK_BLEND_FACTOR_SRC_ALPHA;
	case BlendFactor::ONE_MINUS_SRC_ALPHA:
	    return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	case BlendFactor::DST_ALPHA:
	    return VK_BLEND_FACTOR_DST_ALPHA;
	case BlendFactor::ONE_MINUS_DST_ALPHA:
	    return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	case BlendFactor::CONSTANT_COLOR:
	    return VK_BLEND_FACTOR_CONSTANT_COLOR;
	case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
	    return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
	case BlendFactor::CONSTANT_ALPHA:
	    return VK_BLEND_FACTOR_CONSTANT_ALPHA;
	case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
	    return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
	case BlendFactor::SRC_ALPHA_SATURATE:
	    return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
	case BlendFactor::SRC1_COLOR:
	    return VK_BLEND_FACTOR_SRC1_COLOR;
	case BlendFactor::ONE_MINUS_SRC1_COLOR:
	    return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
	case BlendFactor::SRC1_ALPHA:
	    return VK_BLEND_FACTOR_SRC1_ALPHA;
	case BlendFactor::ONE_MINUS_SRC1_ALPHA:
	    return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
	default:
	    ASSERT(false);
	    break;
    }
    return VkBlendFactor();
}

VkLogicOp VulkanUtilities::Translate(LogicOp logicOp)
{
    switch(logicOp)
    {
	case LogicOp::CLEAR:
	    return VK_LOGIC_OP_CLEAR;
	case LogicOp::AND:
	    return VK_LOGIC_OP_AND;
	case LogicOp::AND_REVERSE:
	    return VK_LOGIC_OP_AND_REVERSE;
	case LogicOp::COPY:
	    return VK_LOGIC_OP_COPY;
	case LogicOp::AND_INVERTED:
	    return VK_LOGIC_OP_AND_INVERTED;
	case LogicOp::NO_OP:
	    return VK_LOGIC_OP_NO_OP;
	case LogicOp::XOR:
	    return VK_LOGIC_OP_XOR;
	case LogicOp::OR:
	    return VK_LOGIC_OP_OR;
	case LogicOp::NOR:
	    return VK_LOGIC_OP_NOR;
	case LogicOp::EQUIVALENT:
	    return VK_LOGIC_OP_EQUIVALENT;
	case LogicOp::INVERT:
	    return VK_LOGIC_OP_INVERT;
	case LogicOp::OR_REVERSE:
	    return VK_LOGIC_OP_OR_REVERSE;
	case LogicOp::COPY_INVERTED:
	    return VK_LOGIC_OP_COPY_INVERTED;
	case LogicOp::OR_INVERTED:
	    return VK_LOGIC_OP_OR_INVERTED;
	case LogicOp::NAND:
	    return VK_LOGIC_OP_NAND;
	case LogicOp::SET:
	    return VK_LOGIC_OP_SET;
	default:
	    ASSERT(false);
	    break;
    }
    return VkLogicOp();
}

VkCompareOp VulkanUtilities::Translate(CompareOp compareOp)
{
    switch(compareOp)
    {
	case CompareOp::NEVER:
	    return VK_COMPARE_OP_NEVER;
	case CompareOp::LESS:
	    return VK_COMPARE_OP_LESS;
	case CompareOp::EQUAL:
	    return VK_COMPARE_OP_EQUAL;
	case CompareOp::LESS_OR_EQUAL:
	    return VK_COMPARE_OP_LESS_OR_EQUAL;
	case CompareOp::GREATER:
	    return VK_COMPARE_OP_GREATER;
	case CompareOp::NOT_EQUAL:
	    return VK_COMPARE_OP_NOT_EQUAL;
	case CompareOp::GREATER_OR_EQUAL:
	    return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case CompareOp::ALWAYS:
	    return VK_COMPARE_OP_ALWAYS;
	default:
	    ASSERT(false);
	    break;
    }
    return VkCompareOp();
}

VkStencilOp VulkanUtilities::Translate(StencilOp stencilOp)
{
    switch(stencilOp)
    {
	case StencilOp::KEEP:
	    return VK_STENCIL_OP_KEEP;
	case StencilOp::ZERO:
	    return VK_STENCIL_OP_ZERO;
	case StencilOp::REPLACE:
	    return VK_STENCIL_OP_REPLACE;
	case StencilOp::INCREMENT_AND_CLAMP:
	    return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
	case StencilOp::DECREMENT_AND_CLAMP:
	    return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
	case StencilOp::INVERT:
	    return VK_STENCIL_OP_INVERT;
	case StencilOp::INCREMENT_AND_WRAP:
	    return VK_STENCIL_OP_INCREMENT_AND_WRAP;
	case StencilOp::DECREMENT_AND_WRAP:
	    return VK_STENCIL_OP_DECREMENT_AND_WRAP;
	default:
	    assert(false);
	    break;
    }
    return VkStencilOp();
}

VkFrontFace VulkanUtilities::Translate(FrontFace frontFace)
{
    switch(frontFace)
    {
	case FrontFace::COUNTER_CLOCKWISE:
	    return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	case FrontFace::CLOCKWISE:
	    return VK_FRONT_FACE_CLOCKWISE;
	default:
	    ASSERT(false);
	    break;
    }
    return VkFrontFace();
}

VkCullModeFlags VulkanUtilities::Translate(CullModeFlags flags)
{
    VkCullModeFlags result = 0;

    if(TestFlagBit(flags, CullModeFlags::NONE))
    {
	result |= VK_CULL_MODE_NONE;
    }
    if(TestFlagBit(flags, CullModeFlags::FRONT_BIT))
    {
	result |= VK_CULL_MODE_FRONT_BIT;
    }
    if(TestFlagBit(flags, CullModeFlags::BACK_BIT))
    {
	result |= VK_CULL_MODE_BACK_BIT;
    }
    if(TestFlagBit(flags, CullModeFlags::FRONT_AND_BACK))
    {
	result |= VK_CULL_MODE_FRONT_AND_BACK;
    }

    return result;
}

VkPolygonMode VulkanUtilities::Translate(PolygonMode polygonMode)
{
    switch(polygonMode)
    {
	case PolygonMode::FILL:
	    return VK_POLYGON_MODE_FILL;
	case PolygonMode::LINE:
	    return VK_POLYGON_MODE_LINE;
	case PolygonMode::POINT:
	    return VK_POLYGON_MODE_POINT;
	default:
	    ASSERT(false);
	    break;
    }
    return VkPolygonMode();
}

VkPrimitiveTopology VulkanUtilities::Translate(PrimitiveTopology primitiveTopology)
{
    switch(primitiveTopology)
    {
	case PrimitiveTopology::POINT_LIST:
	    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case PrimitiveTopology::LINE_LIST:
	    return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case PrimitiveTopology::LINE_STRIP:
	    return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case PrimitiveTopology::TRIANGLE_LIST:
	    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case PrimitiveTopology::TRIANGLE_STRIP:
	    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case PrimitiveTopology::TRIANGLE_FAN:
	    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	case PrimitiveTopology::LINE_LIST_WITH_ADJACENCY:
	    return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
	case PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY:
	    return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
	case PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY:
	    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
	case PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY:
	    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
	case PrimitiveTopology::PATCH_LIST:
	    return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	default:
	    ASSERT(false);
	    break;
    }
    return VkPrimitiveTopology();
}

VkVertexInputRate VulkanUtilities::Translate(VertexInputRate inputRate)
{
    switch(inputRate)
    {
	case VertexInputRate::VERTEX:
	    return VK_VERTEX_INPUT_RATE_VERTEX;
	case VertexInputRate::INSTANCE:
	    return VK_VERTEX_INPUT_RATE_INSTANCE;
	default:
	    ASSERT(false);
	    break;
    }
    return VkVertexInputRate();
}

VkFormat VulkanUtilities::Translate(Format format)
{
    switch(format)
    {
	case Format::UNDEFINED:
	    return VK_FORMAT_UNDEFINED;
	case Format::R4G4_UNORM_PACK8:
	    return VK_FORMAT_R4G4_UNORM_PACK8;
	case Format::R4G4B4A4_UNORM_PACK16:
	    return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
	case Format::B4G4R4A4_UNORM_PACK16:
	    return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	case Format::R5G6B5_UNORM_PACK16:
	    return VK_FORMAT_R5G6B5_UNORM_PACK16;
	case Format::B5G6R5_UNORM_PACK16:
	    return VK_FORMAT_B5G6R5_UNORM_PACK16;
	case Format::R5G5B5A1_UNORM_PACK16:
	    return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
	case Format::B5G5R5A1_UNORM_PACK16:
	    return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case Format::A1R5G5B5_UNORM_PACK16:
	    return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
	case Format::R8_UNORM:
	    return VK_FORMAT_R8_UNORM;
	case Format::R8_SNORM:
	    return VK_FORMAT_R8_SNORM;
	case Format::R8_USCALED:
	    return VK_FORMAT_R8_USCALED;
	case Format::R8_SSCALED:
	    return VK_FORMAT_R8_SSCALED;
	case Format::R8_UINT:
	    return VK_FORMAT_R8_UINT;
	case Format::R8_SINT:
	    return VK_FORMAT_R8_SINT;
	case Format::R8_SRGB:
	    return VK_FORMAT_R8_SRGB;
	case Format::R8G8_UNORM:
	    return VK_FORMAT_R8G8_UNORM;
	case Format::R8G8_SNORM:
	    return VK_FORMAT_R8G8_SNORM;
	case Format::R8G8_USCALED:
	    return VK_FORMAT_R8G8_USCALED;
	case Format::R8G8_SSCALED:
	    return VK_FORMAT_R8G8_SSCALED;
	case Format::R8G8_UINT:
	    return VK_FORMAT_R8G8_UINT;
	case Format::R8G8_SINT:
	    return VK_FORMAT_R8G8_SINT;
	case Format::R8G8_SRGB:
	    return VK_FORMAT_R8G8_SRGB;
	case Format::R8G8B8_UNORM:
	    return VK_FORMAT_R8G8B8_UNORM;
	case Format::R8G8B8_SNORM:
	    return VK_FORMAT_R8G8B8_SNORM;
	case Format::R8G8B8_USCALED:
	    return VK_FORMAT_R8G8B8_USCALED;
	case Format::R8G8B8_SSCALED:
	    return VK_FORMAT_R8G8B8_SSCALED;
	case Format::R8G8B8_UINT:
	    return VK_FORMAT_R8G8B8_UINT;
	case Format::R8G8B8_SINT:
	    return VK_FORMAT_R8G8B8_SINT;
	case Format::R8G8B8_SRGB:
	    return VK_FORMAT_R8G8B8_SRGB;
	case Format::B8G8R8_UNORM:
	    return VK_FORMAT_B8G8R8_UNORM;
	case Format::B8G8R8_SNORM:
	    return VK_FORMAT_B8G8R8_SNORM;
	case Format::B8G8R8_USCALED:
	    return VK_FORMAT_B8G8R8_USCALED;
	case Format::B8G8R8_SSCALED:
	    return VK_FORMAT_B8G8R8_SSCALED;
	case Format::B8G8R8_UINT:
	    return VK_FORMAT_B8G8R8_UINT;
	case Format::B8G8R8_SINT:
	    return VK_FORMAT_B8G8R8_SINT;
	case Format::B8G8R8_SRGB:
	    return VK_FORMAT_B8G8R8_SRGB;
	case Format::R8G8B8A8_UNORM:
	    return VK_FORMAT_R8G8B8A8_UNORM;
	case Format::R8G8B8A8_SNORM:
	    return VK_FORMAT_R8G8B8A8_SNORM;
	case Format::R8G8B8A8_USCALED:
	    return VK_FORMAT_R8G8B8A8_USCALED;
	case Format::R8G8B8A8_SSCALED:
	    return VK_FORMAT_R8G8B8A8_SSCALED;
	case Format::R8G8B8A8_UINT:
	    return VK_FORMAT_R8G8B8A8_UINT;
	case Format::R8G8B8A8_SINT:
	    return VK_FORMAT_R8G8B8A8_SINT;
	case Format::R8G8B8A8_SRGB:
	    return VK_FORMAT_R8G8B8A8_SRGB;
	case Format::B8G8R8A8_UNORM:
	    return VK_FORMAT_B8G8R8A8_UNORM;
	case Format::B8G8R8A8_SNORM:
	    return VK_FORMAT_B8G8R8A8_SNORM;
	case Format::B8G8R8A8_USCALED:
	    return VK_FORMAT_B8G8R8A8_USCALED;
	case Format::B8G8R8A8_SSCALED:
	    return VK_FORMAT_B8G8R8A8_SSCALED;
	case Format::B8G8R8A8_UINT:
	    return VK_FORMAT_B8G8R8A8_UINT;
	case Format::B8G8R8A8_SINT:
	    return VK_FORMAT_B8G8R8A8_SINT;
	case Format::B8G8R8A8_SRGB:
	    return VK_FORMAT_B8G8R8A8_SRGB;
	case Format::A8B8G8R8_UNORM_PACK32:
	    return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
	case Format::A8B8G8R8_SNORM_PACK32:
	    return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
	case Format::A8B8G8R8_USCALED_PACK32:
	    return VK_FORMAT_A8B8G8R8_USCALED_PACK32;
	case Format::A8B8G8R8_SSCALED_PACK32:
	    return VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
	case Format::A8B8G8R8_UINT_PACK32:
	    return VK_FORMAT_A8B8G8R8_UINT_PACK32;
	case Format::A8B8G8R8_SINT_PACK32:
	    return VK_FORMAT_A8B8G8R8_SINT_PACK32;
	case Format::A8B8G8R8_SRGB_PACK32:
	    return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
	case Format::A2R10G10B10_UNORM_PACK32:
	    return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
	case Format::A2R10G10B10_SNORM_PACK32:
	    return VK_FORMAT_A2R10G10B10_SNORM_PACK32;
	case Format::A2R10G10B10_USCALED_PACK32:
	    return VK_FORMAT_A2R10G10B10_USCALED_PACK32;
	case Format::A2R10G10B10_SSCALED_PACK32:
	    return VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
	case Format::A2R10G10B10_UINT_PACK32:
	    return VK_FORMAT_A2R10G10B10_UINT_PACK32;
	case Format::A2R10G10B10_SINT_PACK32:
	    return VK_FORMAT_A2R10G10B10_SINT_PACK32;
	case Format::A2B10G10R10_UNORM_PACK32:
	    return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	case Format::A2B10G10R10_SNORM_PACK32:
	    return VK_FORMAT_A2B10G10R10_SNORM_PACK32;
	case Format::A2B10G10R10_USCALED_PACK32:
	    return VK_FORMAT_A2B10G10R10_USCALED_PACK32;
	case Format::A2B10G10R10_SSCALED_PACK32:
	    return VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
	case Format::A2B10G10R10_UINT_PACK32:
	    return VK_FORMAT_A2B10G10R10_UINT_PACK32;
	case Format::A2B10G10R10_SINT_PACK32:
	    return VK_FORMAT_A2B10G10R10_SINT_PACK32;
	case Format::R16_UNORM:
	    return VK_FORMAT_R16_UNORM;
	case Format::R16_SNORM:
	    return VK_FORMAT_R16_SNORM;
	case Format::R16_USCALED:
	    return VK_FORMAT_R16_USCALED;
	case Format::R16_SSCALED:
	    return VK_FORMAT_R16_SSCALED;
	case Format::R16_UINT:
	    return VK_FORMAT_R16_UINT;
	case Format::R16_SINT:
	    return VK_FORMAT_R16_SINT;
	case Format::R16_SFLOAT:
	    return VK_FORMAT_R16_SFLOAT;
	case Format::R16G16_UNORM:
	    return VK_FORMAT_R16G16_UNORM;
	case Format::R16G16_SNORM:
	    return VK_FORMAT_R16G16_SNORM;
	case Format::R16G16_USCALED:
	    return VK_FORMAT_R16G16_USCALED;
	case Format::R16G16_SSCALED:
	    return VK_FORMAT_R16G16_SSCALED;
	case Format::R16G16_UINT:
	    return VK_FORMAT_R16G16_UINT;
	case Format::R16G16_SINT:
	    return VK_FORMAT_R16G16_SINT;
	case Format::R16G16_SFLOAT:
	    return VK_FORMAT_R16G16_SFLOAT;
	case Format::R16G16B16_UNORM:
	    return VK_FORMAT_R16G16B16_UNORM;
	case Format::R16G16B16_SNORM:
	    return VK_FORMAT_R16G16B16_SNORM;
	case Format::R16G16B16_USCALED:
	    return VK_FORMAT_R16G16B16_USCALED;
	case Format::R16G16B16_SSCALED:
	    return VK_FORMAT_R16G16B16_SSCALED;
	case Format::R16G16B16_UINT:
	    return VK_FORMAT_R16G16B16_UINT;
	case Format::R16G16B16_SINT:
	    return VK_FORMAT_R16G16B16_SINT;
	case Format::R16G16B16_SFLOAT:
	    return VK_FORMAT_R16G16B16_SFLOAT;
	case Format::R16G16B16A16_UNORM:
	    return VK_FORMAT_R16G16B16A16_UNORM;
	case Format::R16G16B16A16_SNORM:
	    return VK_FORMAT_R16G16B16A16_SNORM;
	case Format::R16G16B16A16_USCALED:
	    return VK_FORMAT_R16G16B16A16_USCALED;
	case Format::R16G16B16A16_SSCALED:
	    return VK_FORMAT_R16G16B16A16_SSCALED;
	case Format::R16G16B16A16_UINT:
	    return VK_FORMAT_R16G16B16A16_UINT;
	case Format::R16G16B16A16_SINT:
	    return VK_FORMAT_R16G16B16A16_SINT;
	case Format::R16G16B16A16_SFLOAT:
	    return VK_FORMAT_R16G16B16A16_SFLOAT;
	case Format::R32_UINT:
	    return VK_FORMAT_R32_UINT;
	case Format::R32_SINT:
	    return VK_FORMAT_R32_SINT;
	case Format::R32_SFLOAT:
	    return VK_FORMAT_R32_SFLOAT;
	case Format::R32G32_UINT:
	    return VK_FORMAT_R32G32_UINT;
	case Format::R32G32_SINT:
	    return VK_FORMAT_R32G32_SINT;
	case Format::R32G32_SFLOAT:
	    return VK_FORMAT_R32G32_SFLOAT;
	case Format::R32G32B32_UINT:
	    return VK_FORMAT_R32G32B32_UINT;
	case Format::R32G32B32_SINT:
	    return VK_FORMAT_R32G32B32_SINT;
	case Format::R32G32B32_SFLOAT:
	    return VK_FORMAT_R32G32B32_SFLOAT;
	case Format::R32G32B32A32_UINT:
	    return VK_FORMAT_R32G32B32A32_UINT;
	case Format::R32G32B32A32_SINT:
	    return VK_FORMAT_R32G32B32A32_SINT;
	case Format::R32G32B32A32_SFLOAT:
	    return VK_FORMAT_R32G32B32A32_SFLOAT;
	case Format::R64_UINT:
	    return VK_FORMAT_R64_UINT;
	case Format::R64_SINT:
	    return VK_FORMAT_R64_SINT;
	case Format::R64_SFLOAT:
	    return VK_FORMAT_R64_SFLOAT;
	case Format::R64G64_UINT:
	    return VK_FORMAT_R64G64_UINT;
	case Format::R64G64_SINT:
	    return VK_FORMAT_R64G64_SINT;
	case Format::R64G64_SFLOAT:
	    return VK_FORMAT_R64G64_SFLOAT;
	case Format::R64G64B64_UINT:
	    return VK_FORMAT_R64G64B64_UINT;
	case Format::R64G64B64_SINT:
	    return VK_FORMAT_R64G64B64_SINT;
	case Format::R64G64B64_SFLOAT:
	    return VK_FORMAT_R64G64B64_SFLOAT;
	case Format::R64G64B64A64_UINT:
	    return VK_FORMAT_R64G64B64A64_UINT;
	case Format::R64G64B64A64_SINT:
	    return VK_FORMAT_R64G64B64A64_SINT;
	case Format::R64G64B64A64_SFLOAT:
	    return VK_FORMAT_R64G64B64A64_SFLOAT;
	case Format::B10G11R11_UFLOAT_PACK32:
	    return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
	case Format::E5B9G9R9_UFLOAT_PACK32:
	    return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
	case Format::D16_UNORM:
	    return VK_FORMAT_D16_UNORM;
	case Format::X8_D24_UNORM_PACK32:
	    return VK_FORMAT_X8_D24_UNORM_PACK32;
	case Format::D32_SFLOAT:
	    return VK_FORMAT_D32_SFLOAT;
	case Format::S8_UINT:
	    return VK_FORMAT_S8_UINT;
	case Format::D16_UNORM_S8_UINT:
	    return VK_FORMAT_D16_UNORM_S8_UINT;
	case Format::D24_UNORM_S8_UINT:
	    return VK_FORMAT_D24_UNORM_S8_UINT;
	case Format::D32_SFLOAT_S8_UINT:
	    return VK_FORMAT_D32_SFLOAT_S8_UINT;
	case Format::BC1_RGB_UNORM_BLOCK:
	    return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
	case Format::BC1_RGB_SRGB_BLOCK:
	    return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
	case Format::BC1_RGBA_UNORM_BLOCK:
	    return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case Format::BC1_RGBA_SRGB_BLOCK:
	    return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
	case Format::BC2_UNORM_BLOCK:
	    return VK_FORMAT_BC2_UNORM_BLOCK;
	case Format::BC2_SRGB_BLOCK:
	    return VK_FORMAT_BC2_SRGB_BLOCK;
	case Format::BC3_UNORM_BLOCK:
	    return VK_FORMAT_BC3_UNORM_BLOCK;
	case Format::BC3_SRGB_BLOCK:
	    return VK_FORMAT_BC3_SRGB_BLOCK;
	case Format::BC4_UNORM_BLOCK:
	    return VK_FORMAT_BC4_UNORM_BLOCK;
	case Format::BC4_SNORM_BLOCK:
	    return VK_FORMAT_BC4_SNORM_BLOCK;
	case Format::BC5_UNORM_BLOCK:
	    return VK_FORMAT_BC5_UNORM_BLOCK;
	case Format::BC5_SNORM_BLOCK:
	    return VK_FORMAT_BC5_SNORM_BLOCK;
	case Format::BC6H_UFLOAT_BLOCK:
	    return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case Format::BC6H_SFLOAT_BLOCK:
	    return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case Format::BC7_UNORM_BLOCK:
	    return VK_FORMAT_BC7_UNORM_BLOCK;
	case Format::BC7_SRGB_BLOCK:
	    return VK_FORMAT_BC7_SRGB_BLOCK;
	default:
	    ASSERT(false);
	    break;
    }
    return VkFormat();
}

Format VulkanUtilities::Translate(VkFormat format)
{
    switch(format)
    {
	case VK_FORMAT_UNDEFINED:
	    return Format::UNDEFINED;
	case VK_FORMAT_R4G4_UNORM_PACK8:
	    return Format::R4G4_UNORM_PACK8;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
	    return Format::R4G4B4A4_UNORM_PACK16;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
	    return Format::B4G4R4A4_UNORM_PACK16;
	case VK_FORMAT_R5G6B5_UNORM_PACK16:
	    return Format::R5G6B5_UNORM_PACK16;
	case VK_FORMAT_B5G6R5_UNORM_PACK16:
	    return Format::B5G6R5_UNORM_PACK16;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
	    return Format::R5G5B5A1_UNORM_PACK16;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
	    return Format::B5G5R5A1_UNORM_PACK16;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
	    return Format::A1R5G5B5_UNORM_PACK16;
	case VK_FORMAT_R8_UNORM:
	    return Format::R8_UNORM;
	case VK_FORMAT_R8_SNORM:
	    return Format::R8_SNORM;
	case VK_FORMAT_R8_USCALED:
	    return Format::R8_USCALED;
	case VK_FORMAT_R8_SSCALED:
	    return Format::R8_SSCALED;
	case VK_FORMAT_R8_UINT:
	    return Format::R8_UINT;
	case VK_FORMAT_R8_SINT:
	    return Format::R8_SINT;
	case VK_FORMAT_R8_SRGB:
	    return Format::R8_SRGB;
	case VK_FORMAT_R8G8_UNORM:
	    return Format::R8G8_UNORM;
	case VK_FORMAT_R8G8_SNORM:
	    return Format::R8G8_SNORM;
	case VK_FORMAT_R8G8_USCALED:
	    return Format::R8G8_USCALED;
	case VK_FORMAT_R8G8_SSCALED:
	    return Format::R8G8_SSCALED;
	case VK_FORMAT_R8G8_UINT:
	    return Format::R8G8_UINT;
	case VK_FORMAT_R8G8_SINT:
	    return Format::R8G8_SINT;
	case VK_FORMAT_R8G8_SRGB:
	    return Format::R8G8_SRGB;
	case VK_FORMAT_R8G8B8_UNORM:
	    return Format::R8G8B8_UNORM;
	case VK_FORMAT_R8G8B8_SNORM:
	    return Format::R8G8B8_SNORM;
	case VK_FORMAT_R8G8B8_USCALED:
	    return Format::R8G8B8_USCALED;
	case VK_FORMAT_R8G8B8_SSCALED:
	    return Format::R8G8B8_SSCALED;
	case VK_FORMAT_R8G8B8_UINT:
	    return Format::R8G8B8_UINT;
	case VK_FORMAT_R8G8B8_SINT:
	    return Format::R8G8B8_SINT;
	case VK_FORMAT_R8G8B8_SRGB:
	    return Format::R8G8B8_SRGB;
	case VK_FORMAT_B8G8R8_UNORM:
	    return Format::B8G8R8_UNORM;
	case VK_FORMAT_B8G8R8_SNORM:
	    return Format::B8G8R8_SNORM;
	case VK_FORMAT_B8G8R8_USCALED:
	    return Format::B8G8R8_USCALED;
	case VK_FORMAT_B8G8R8_SSCALED:
	    return Format::B8G8R8_SSCALED;
	case VK_FORMAT_B8G8R8_UINT:
	    return Format::B8G8R8_UINT;
	case VK_FORMAT_B8G8R8_SINT:
	    return Format::B8G8R8_SINT;
	case VK_FORMAT_B8G8R8_SRGB:
	    return Format::B8G8R8_SRGB;
	case VK_FORMAT_R8G8B8A8_UNORM:
	    return Format::R8G8B8A8_UNORM;
	case VK_FORMAT_R8G8B8A8_SNORM:
	    return Format::R8G8B8A8_SNORM;
	case VK_FORMAT_R8G8B8A8_USCALED:
	    return Format::R8G8B8A8_USCALED;
	case VK_FORMAT_R8G8B8A8_SSCALED:
	    return Format::R8G8B8A8_SSCALED;
	case VK_FORMAT_R8G8B8A8_UINT:
	    return Format::R8G8B8A8_UINT;
	case VK_FORMAT_R8G8B8A8_SINT:
	    return Format::R8G8B8A8_SINT;
	case VK_FORMAT_R8G8B8A8_SRGB:
	    return Format::R8G8B8A8_SRGB;
	case VK_FORMAT_B8G8R8A8_UNORM:
	    return Format::B8G8R8A8_UNORM;
	case VK_FORMAT_B8G8R8A8_SNORM:
	    return Format::B8G8R8A8_SNORM;
	case VK_FORMAT_B8G8R8A8_USCALED:
	    return Format::B8G8R8A8_USCALED;
	case VK_FORMAT_B8G8R8A8_SSCALED:
	    return Format::B8G8R8A8_SSCALED;
	case VK_FORMAT_B8G8R8A8_UINT:
	    return Format::B8G8R8A8_UINT;
	case VK_FORMAT_B8G8R8A8_SINT:
	    return Format::B8G8R8A8_SINT;
	case VK_FORMAT_B8G8R8A8_SRGB:
	    return Format::B8G8R8A8_SRGB;
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
	    return Format::A8B8G8R8_UNORM_PACK32;
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
	    return Format::A8B8G8R8_SNORM_PACK32;
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
	    return Format::A8B8G8R8_USCALED_PACK32;
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
	    return Format::A8B8G8R8_SSCALED_PACK32;
	case VK_FORMAT_A8B8G8R8_UINT_PACK32:
	    return Format::A8B8G8R8_UINT_PACK32;
	case VK_FORMAT_A8B8G8R8_SINT_PACK32:
	    return Format::A8B8G8R8_SINT_PACK32;
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
	    return Format::A8B8G8R8_SRGB_PACK32;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
	    return Format::A2R10G10B10_UNORM_PACK32;
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
	    return Format::A2R10G10B10_SNORM_PACK32;
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
	    return Format::A2R10G10B10_USCALED_PACK32;
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
	    return Format::A2R10G10B10_SSCALED_PACK32;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32:
	    return Format::A2R10G10B10_UINT_PACK32;
	case VK_FORMAT_A2R10G10B10_SINT_PACK32:
	    return Format::A2R10G10B10_SINT_PACK32;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
	    return Format::A2B10G10R10_UNORM_PACK32;
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
	    return Format::A2B10G10R10_SNORM_PACK32;
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
	    return Format::A2B10G10R10_USCALED_PACK32;
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
	    return Format::A2B10G10R10_SSCALED_PACK32;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32:
	    return Format::A2B10G10R10_UINT_PACK32;
	case VK_FORMAT_A2B10G10R10_SINT_PACK32:
	    return Format::A2B10G10R10_SINT_PACK32;
	case VK_FORMAT_R16_UNORM:
	    return Format::R16_UNORM;
	case VK_FORMAT_R16_SNORM:
	    return Format::R16_SNORM;
	case VK_FORMAT_R16_USCALED:
	    return Format::R16_USCALED;
	case VK_FORMAT_R16_SSCALED:
	    return Format::R16_SSCALED;
	case VK_FORMAT_R16_UINT:
	    return Format::R16_UINT;
	case VK_FORMAT_R16_SINT:
	    return Format::R16_SINT;
	case VK_FORMAT_R16_SFLOAT:
	    return Format::R16_SFLOAT;
	case VK_FORMAT_R16G16_UNORM:
	    return Format::R16G16_UNORM;
	case VK_FORMAT_R16G16_SNORM:
	    return Format::R16G16_SNORM;
	case VK_FORMAT_R16G16_USCALED:
	    return Format::R16G16_USCALED;
	case VK_FORMAT_R16G16_SSCALED:
	    return Format::R16G16_SSCALED;
	case VK_FORMAT_R16G16_UINT:
	    return Format::R16G16_UINT;
	case VK_FORMAT_R16G16_SINT:
	    return Format::R16G16_SINT;
	case VK_FORMAT_R16G16_SFLOAT:
	    return Format::R16G16_SFLOAT;
	case VK_FORMAT_R16G16B16_UNORM:
	    return Format::R16G16B16_UNORM;
	case VK_FORMAT_R16G16B16_SNORM:
	    return Format::R16G16B16_SNORM;
	case VK_FORMAT_R16G16B16_USCALED:
	    return Format::R16G16B16_USCALED;
	case VK_FORMAT_R16G16B16_SSCALED:
	    return Format::R16G16B16_SSCALED;
	case VK_FORMAT_R16G16B16_UINT:
	    return Format::R16G16B16_UINT;
	case VK_FORMAT_R16G16B16_SINT:
	    return Format::R16G16B16_SINT;
	case VK_FORMAT_R16G16B16_SFLOAT:
	    return Format::R16G16B16_SFLOAT;
	case VK_FORMAT_R16G16B16A16_UNORM:
	    return Format::R16G16B16A16_UNORM;
	case VK_FORMAT_R16G16B16A16_SNORM:
	    return Format::R16G16B16A16_SNORM;
	case VK_FORMAT_R16G16B16A16_USCALED:
	    return Format::R16G16B16A16_USCALED;
	case VK_FORMAT_R16G16B16A16_SSCALED:
	    return Format::R16G16B16A16_SSCALED;
	case VK_FORMAT_R16G16B16A16_UINT:
	    return Format::R16G16B16A16_UINT;
	case VK_FORMAT_R16G16B16A16_SINT:
	    return Format::R16G16B16A16_SINT;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
	    return Format::R16G16B16A16_SFLOAT;
	case VK_FORMAT_R32_UINT:
	    return Format::R32_UINT;
	case VK_FORMAT_R32_SINT:
	    return Format::R32_SINT;
	case VK_FORMAT_R32_SFLOAT:
	    return Format::R32_SFLOAT;
	case VK_FORMAT_R32G32_UINT:
	    return Format::R32G32_UINT;
	case VK_FORMAT_R32G32_SINT:
	    return Format::R32G32_SINT;
	case VK_FORMAT_R32G32_SFLOAT:
	    return Format::R32G32_SFLOAT;
	case VK_FORMAT_R32G32B32_UINT:
	    return Format::R32G32B32_UINT;
	case VK_FORMAT_R32G32B32_SINT:
	    return Format::R32G32B32_SINT;
	case VK_FORMAT_R32G32B32_SFLOAT:
	    return Format::R32G32B32_SFLOAT;
	case VK_FORMAT_R32G32B32A32_UINT:
	    return Format::R32G32B32A32_UINT;
	case VK_FORMAT_R32G32B32A32_SINT:
	    return Format::R32G32B32A32_SINT;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
	    return Format::R32G32B32A32_SFLOAT;
	case VK_FORMAT_R64_UINT:
	    return Format::R64_UINT;
	case VK_FORMAT_R64_SINT:
	    return Format::R64_SINT;
	case VK_FORMAT_R64_SFLOAT:
	    return Format::R64_SFLOAT;
	case VK_FORMAT_R64G64_UINT:
	    return Format::R64G64_UINT;
	case VK_FORMAT_R64G64_SINT:
	    return Format::R64G64_SINT;
	case VK_FORMAT_R64G64_SFLOAT:
	    return Format::R64G64_SFLOAT;
	case VK_FORMAT_R64G64B64_UINT:
	    return Format::R64G64B64_UINT;
	case VK_FORMAT_R64G64B64_SINT:
	    return Format::R64G64B64_SINT;
	case VK_FORMAT_R64G64B64_SFLOAT:
	    return Format::R64G64B64_SFLOAT;
	case VK_FORMAT_R64G64B64A64_UINT:
	    return Format::R64G64B64A64_UINT;
	case VK_FORMAT_R64G64B64A64_SINT:
	    return Format::R64G64B64A64_SINT;
	case VK_FORMAT_R64G64B64A64_SFLOAT:
	    return Format::R64G64B64A64_SFLOAT;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
	    return Format::B10G11R11_UFLOAT_PACK32;
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
	    return Format::E5B9G9R9_UFLOAT_PACK32;
	case VK_FORMAT_D16_UNORM:
	    return Format::D16_UNORM;
	case VK_FORMAT_X8_D24_UNORM_PACK32:
	    return Format::X8_D24_UNORM_PACK32;
	case VK_FORMAT_D32_SFLOAT:
	    return Format::D32_SFLOAT;
	case VK_FORMAT_S8_UINT:
	    return Format::S8_UINT;
	case VK_FORMAT_D16_UNORM_S8_UINT:
	    return Format::D16_UNORM_S8_UINT;
	case VK_FORMAT_D24_UNORM_S8_UINT:
	    return Format::D24_UNORM_S8_UINT;
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
	    return Format::D32_SFLOAT_S8_UINT;
	case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
	    return Format::BC1_RGB_UNORM_BLOCK;
	case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
	    return Format::BC1_RGB_SRGB_BLOCK;
	case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
	    return Format::BC1_RGBA_UNORM_BLOCK;
	case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
	    return Format::BC1_RGBA_SRGB_BLOCK;
	case VK_FORMAT_BC2_UNORM_BLOCK:
	    return Format::BC2_UNORM_BLOCK;
	case VK_FORMAT_BC2_SRGB_BLOCK:
	    return Format::BC2_SRGB_BLOCK;
	case VK_FORMAT_BC3_UNORM_BLOCK:
	    return Format::BC3_UNORM_BLOCK;
	case VK_FORMAT_BC3_SRGB_BLOCK:
	    return Format::BC3_SRGB_BLOCK;
	case VK_FORMAT_BC4_UNORM_BLOCK:
	    return Format::BC4_UNORM_BLOCK;
	case VK_FORMAT_BC4_SNORM_BLOCK:
	    return Format::BC4_SNORM_BLOCK;
	case VK_FORMAT_BC5_UNORM_BLOCK:
	    return Format::BC5_UNORM_BLOCK;
	case VK_FORMAT_BC5_SNORM_BLOCK:
	    return Format::BC5_SNORM_BLOCK;
	case VK_FORMAT_BC6H_UFLOAT_BLOCK:
	    return Format::BC6H_UFLOAT_BLOCK;
	case VK_FORMAT_BC6H_SFLOAT_BLOCK:
	    return Format::BC6H_SFLOAT_BLOCK;
	case VK_FORMAT_BC7_UNORM_BLOCK:
	    return Format::BC7_UNORM_BLOCK;
	case VK_FORMAT_BC7_SRGB_BLOCK:
	    return Format::BC7_SRGB_BLOCK;
	default:
	    ASSERT(false);
	    break;
    }
    return Format();
}

VkImageType VulkanUtilities::Translate(ImageType imageType)
{
    switch(imageType)
    {
	case ImageType::_1D:
	    return VK_IMAGE_TYPE_1D;
	case ImageType::_2D:
	    return VK_IMAGE_TYPE_2D;
	case ImageType::_3D:
	    return VK_IMAGE_TYPE_3D;
	default:
	    assert(false);
	    break;
    }
    return VkImageType();
}

VkImageViewType VulkanUtilities::Translate(ImageViewType imageViewType)
{
    switch(imageViewType)
    {
	case ImageViewType::_1D:
	    return VK_IMAGE_VIEW_TYPE_1D;
	case ImageViewType::_2D:
	    return VK_IMAGE_VIEW_TYPE_2D;
	case ImageViewType::_3D:
	    return VK_IMAGE_VIEW_TYPE_3D;
	case ImageViewType::CUBE:
	    return VK_IMAGE_VIEW_TYPE_CUBE;
	case ImageViewType::_1D_ARRAY:
	    return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
	case ImageViewType::_2D_ARRAY:
	    return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	case ImageViewType::CUBE_ARRAY:
	    return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
	default:
	    assert(false);
	    break;
    }
    return VkImageViewType();
}

VkBufferCreateFlags VulkanUtilities::Translate(BufferCreateFlags flags)
{
    VkBufferCreateFlags result = 0;

    return result;
}

VkBufferUsageFlags VulkanUtilities::Translate(BufferUsageFlags flags)
{
    VkBufferUsageFlags result = 0;

    if(TestFlagBit(flags, BufferUsageFlags::TRANSFER_SRC_BIT))
    {
	result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::TRANSFER_DST_BIT))
    {
	result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::TYPED_BUFFER_BIT))
    {
	result |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::RW_TYPED_BUFFER_BIT))
    {
	result |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::CONSTANT_BUFFER_BIT))
    {
	result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::BYTE_BUFFER_BIT))
    {
	result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::RW_BYTE_BUFFER_BIT))
    {
	result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::STRUCTURED_BUFFER_BIT))
    {
	result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::RW_STRUCTURED_BUFFER_BIT))
    {
	result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::INDEX_BUFFER_BIT))
    {
	result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::VERTEX_BUFFER_BIT))
    {
	result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::INDIRECT_BUFFER_BIT))
    {
	result |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::CLEAR_BIT))
    {
	result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    if(TestFlagBit(flags, BufferUsageFlags::SHADER_DEVICE_ADDRESS_BIT))
    {
	result |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    }

    return result;
}

VkComponentSwizzle VulkanUtilities::Translate(ComponentSwizzle swizzle)
{
    switch(swizzle)
    {
	case ComponentSwizzle::IDENTITY:
	    return VK_COMPONENT_SWIZZLE_IDENTITY;
	case ComponentSwizzle::ZERO:
	    return VK_COMPONENT_SWIZZLE_ZERO;
	case ComponentSwizzle::ONE:
	    return VK_COMPONENT_SWIZZLE_ONE;
	case ComponentSwizzle::R:
	    return VK_COMPONENT_SWIZZLE_R;
	case ComponentSwizzle::G:
	    return VK_COMPONENT_SWIZZLE_G;
	case ComponentSwizzle::B:
	    return VK_COMPONENT_SWIZZLE_B;
	case ComponentSwizzle::A:
	    return VK_COMPONENT_SWIZZLE_A;
	default:
	    assert(false);
	    break;
    }
    return VkComponentSwizzle();
}

VkFilter VulkanUtilities::Translate(Filter filter)
{
    switch(filter)
    {
	case Filter::NEAREST:
	    return VK_FILTER_NEAREST;
	case Filter::LINEAR:
	    return VK_FILTER_LINEAR;
	default:
	    assert(false);
	    break;
    }
    return VkFilter();
}

VkImageAspectFlags VulkanUtilities::GetImageAspectMask(VkFormat format)
{
    switch(format)
    {
	case VK_FORMAT_D16_UNORM:
	case VK_FORMAT_X8_D24_UNORM_PACK32:
	case VK_FORMAT_D32_SFLOAT:
	    return VK_IMAGE_ASPECT_DEPTH_BIT;

	case VK_FORMAT_S8_UINT:
	    return VK_IMAGE_ASPECT_STENCIL_BIT;

	case VK_FORMAT_D16_UNORM_S8_UINT:
	case VK_FORMAT_D24_UNORM_S8_UINT:
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
	    return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	default:
	    return VK_IMAGE_ASPECT_COLOR_BIT;
    }

    return VK_IMAGE_ASPECT_COLOR_BIT;
}

VkSamplerMipmapMode VulkanUtilities::Translate(SamplerMipmapMode mipmapMode)
{
    switch(mipmapMode)
    {
	case SamplerMipmapMode::NEAREST:
	    return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case SamplerMipmapMode::LINEAR:
	    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	default:
	    assert(false);
	    break;
    }
    return VkSamplerMipmapMode();
}

VkSamplerAddressMode VulkanUtilities::Translate(SamplerAddressMode addressMode)
{
    switch(addressMode)
    {
	case SamplerAddressMode::REPEAT:
	    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case SamplerAddressMode::MIRRORED_REPEAT:
	    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	case SamplerAddressMode::CLAMP_TO_EDGE:
	    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case SamplerAddressMode::CLAMP_TO_BORDER:
	    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	case SamplerAddressMode::MIRROR_CLAMP_TO_EDGE:
	    return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
	default:
	    assert(false);
	    break;
    }
    return VkSamplerAddressMode();
}

VkBorderColor VulkanUtilities::Translate(BorderColor borderColor)
{
    switch(borderColor)
    {
	case BorderColor::FLOAT_TRANSPARENT_BLACK:
	    return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
	case BorderColor::INT_TRANSPARENT_BLACK:
	    return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
	case BorderColor::FLOAT_OPAQUE_BLACK:
	    return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	case BorderColor::INT_OPAQUE_BLACK:
	    return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	case BorderColor::FLOAT_OPAQUE_WHITE:
	    return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	case BorderColor::INT_OPAQUE_WHITE:
	    return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	default:
	    assert(false);
	    break;
    }
    return VkBorderColor();
}

VkStencilFaceFlags VulkanUtilities::Translate(StencilFaceFlags flags)
{
    VkStencilFaceFlags result = 0;

    if(TestFlagBit(flags, StencilFaceFlags::FRONT_BIT))
    {
	result |= VK_STENCIL_FACE_FRONT_BIT;
    }
    if(TestFlagBit(flags, StencilFaceFlags::BACK_BIT))
    {
	result |= VK_STENCIL_FACE_BACK_BIT;
    }
    if(TestFlagBit(flags, StencilFaceFlags::FRONT_AND_BACK))
    {
	result |= VK_STENCIL_FACE_FRONT_AND_BACK;
    }

    return VkStencilFaceFlags();
}

VkAttachmentLoadOp VulkanUtilities::Translate(AttachmentLoadOp loadOp)
{
    switch(loadOp)
    {
	case AttachmentLoadOp::LOAD:
	    return VK_ATTACHMENT_LOAD_OP_LOAD;
	case AttachmentLoadOp::CLEAR:
	    return VK_ATTACHMENT_LOAD_OP_CLEAR;
	case AttachmentLoadOp::DONT_CARE:
	    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	default:
	    ASSERT(false);
	    break;
    }
    return VkAttachmentLoadOp();
}

VkAttachmentStoreOp VulkanUtilities::Translate(AttachmentStoreOp storeOp)
{
    switch(storeOp)
    {
	case AttachmentStoreOp::STORE:
	    return VK_ATTACHMENT_STORE_OP_STORE;
	case AttachmentStoreOp::DONT_CARE:
	    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	default:
	    ASSERT(false);
	    break;
    }
    return VkAttachmentStoreOp();
}

VkImageCreateFlags VulkanUtilities::Translate(ImageCreateFlags flags)
{
    VkImageCreateFlags result = 0;

    if(TestFlagBit(flags, ImageCreateFlags::MUTABLE_FORMAT_BIT))
    {
	result |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
    }
    if(TestFlagBit(flags, ImageCreateFlags::CUBE_COMPATIBLE_BIT))
    {
	result |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }
    if(TestFlagBit(flags, ImageCreateFlags::_2D_ARRAY_COMPATIBLE_BIT))
    {
	result |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
    }

    return result;
}

VkMemoryPropertyFlags VulkanUtilities::Translate(MemoryPropertyFlags flags)
{
    VkMemoryPropertyFlags result = 0;

    if(TestFlagBit(flags, MemoryPropertyFlags::DEVICE_LOCAL_BIT))
    {
	result |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
    if(TestFlagBit(flags, MemoryPropertyFlags::HOST_VISIBLE_BIT))
    {
	result |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    }
    if(TestFlagBit(flags, MemoryPropertyFlags::HOST_COHERENT_BIT))
    {
	result |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    if(TestFlagBit(flags, MemoryPropertyFlags::HOST_CACHED_BIT))
    {
	result |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    }

    return result;
}

VkPipelineStageFlags VulkanUtilities::Translate(PipelineStageFlags flags)
{
    VkPipelineStageFlags result = 0;

    if(TestFlagBit(flags, PipelineStageFlags::TOP_OF_PIPE_BIT))
    {
	result |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::DRAW_INDIRECT_BIT))
    {
	result |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::VERTEX_INPUT_BIT))
    {
	result |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::VERTEX_SHADER_BIT))
    {
	result |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::HULL_SHADER_BIT))
    {
	result |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::DOMAIN_SHADER_BIT))
    {
	result |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::GEOMETRY_SHADER_BIT))
    {
	result |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::PIXEL_SHADER_BIT))
    {
	result |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::EARLY_FRAGMENT_TESTS_BIT))
    {
	result |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::LATE_FRAGMENT_TESTS_BIT))
    {
	result |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT_BIT))
    {
	result |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::COMPUTE_SHADER_BIT))
    {
	result |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::TRANSFER_BIT))
    {
	result |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::BOTTOM_OF_PIPE_BIT))
    {
	result |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::HOST_BIT))
    {
	result |= VK_PIPELINE_STAGE_HOST_BIT;
    }
    if(TestFlagBit(flags, PipelineStageFlags::CLEAR_BIT))
    {
	result |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    }

    return result;
}

VkImageUsageFlags VulkanUtilities::Translate(ImageUsageFlags flags)
{
    VkImageUsageFlags result = 0;

    if(TestFlagBit(flags, ImageUsageFlags::TRANSFER_SRC_BIT))
    {
	result |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    if(TestFlagBit(flags, ImageUsageFlags::TRANSFER_DST_BIT))
    {
	result |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }
    if(TestFlagBit(flags, ImageUsageFlags::TEXTURE_BIT))
    {
	result |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    if(TestFlagBit(flags, ImageUsageFlags::RW_TEXTURE_BIT))
    {
	result |= VK_IMAGE_USAGE_STORAGE_BIT;
    }
    if(TestFlagBit(flags, ImageUsageFlags::COLOR_ATTACHMENT_BIT))
    {
	result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
    if(TestFlagBit(flags, ImageUsageFlags::DEPTH_STENCIL_ATTACHMENT_BIT))
    {
	result |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    if(TestFlagBit(flags, ImageUsageFlags::CLEAR_BIT))
    {
	result |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    return result;
}

VkDescriptorBindingFlags VulkanUtilities::Translate(DescriptorBindingFlags flags)
{
    VkDescriptorBindingFlags result = 0;

    if(TestFlagBit(flags, DescriptorBindingFlags::UPDATE_AFTER_BIND_BIT))
    {
	result |= VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
    }
    if(TestFlagBit(flags, DescriptorBindingFlags::UPDATE_UNUSED_WHILE_PENDING_BIT))
    {
	result |= VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
    }
    if(TestFlagBit(flags, DescriptorBindingFlags::PARTIALLY_BOUND_BIT))
    {
	result |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    }

    return result;
}

VkIndexType VulkanUtilities::Translate(IndexType indexType)
{
    switch(indexType)
    {
	case IndexType::UINT16:
	    return VK_INDEX_TYPE_UINT16;
	case IndexType::UINT32:
	    return VK_INDEX_TYPE_UINT32;
	default:
	    assert(false);
	    break;
    }
    return VkIndexType();
}