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

    if(errorMsg)
    {
	LOG_CORE_CRITICAL("{0}", msgBuffer);
    }

    bool error = false;

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
	    strcat_s(msgBuffer, " VK_ERROR_OUT_OF_HOST_MEMORY\n");
	    error = true;
	    break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
	    strcat_s(msgBuffer, " VK_ERROR_OUT_OF_DEVICE_MEMORY\n");
	    error = true;
	    break;
	case VK_ERROR_INITIALIZATION_FAILED:
	    strcat_s(msgBuffer, " VK_ERROR_INITIALIZATION_FAILED\n");
	    error = true;
	    break;
	case VK_ERROR_DEVICE_LOST:
	    strcat_s(msgBuffer, " VK_ERROR_DEVICE_LOST\n");
	    error = true;
	    break;
	case VK_ERROR_MEMORY_MAP_FAILED:
	    strcat_s(msgBuffer, " VK_ERROR_MEMORY_MAP_FAILED\n");
	    error = true;
	    break;
	case VK_ERROR_LAYER_NOT_PRESENT:
	    strcat_s(msgBuffer, " VK_ERROR_LAYER_NOT_PRESENT\n");
	    error = true;
	    break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
	    strcat_s(msgBuffer, " VK_ERROR_EXTENSION_NOT_PRESENT\n");
	    error = true;
	    break;
	case VK_ERROR_FEATURE_NOT_PRESENT:
	    strcat_s(msgBuffer, " VK_ERROR_FEATURE_NOT_PRESENT\n");
	    error = true;
	    break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
	    strcat_s(msgBuffer, " VK_ERROR_INCOMPATIBLE_DRIVER\n");
	    error = true;
	    break;
	case VK_ERROR_TOO_MANY_OBJECTS:
	    strcat_s(msgBuffer, " VK_ERROR_TOO_MANY_OBJECTS\n");
	    error = true;
	    break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
	    strcat_s(msgBuffer, " VK_ERROR_FORMAT_NOT_SUPPORTED\n");
	    error = true;
	    break;
	case VK_ERROR_FRAGMENTED_POOL:
	    strcat_s(msgBuffer, " VK_ERROR_FRAGMENTED_POOL\n");
	    error = true;
	    break;
	case VK_ERROR_UNKNOWN:
	    strcat_s(msgBuffer, " VK_ERROR_UNKNOWN\n");
	    error = true;
	    break;
	case VK_ERROR_OUT_OF_POOL_MEMORY:
	    strcat_s(msgBuffer, " VK_ERROR_OUT_OF_POOL_MEMORY\n");
	    error = true;
	    break;
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
	    strcat_s(msgBuffer, " VK_ERROR_INVALID_EXTERNAL_HANDLE\n");
	    error = true;
	    break;
	case VK_ERROR_FRAGMENTATION:
	    strcat_s(msgBuffer, " VK_ERROR_FRAGMENTATION\n");
	    error = true;
	    break;
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
	    strcat_s(msgBuffer, " VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS\n");
	    error = true;
	    break;
	case VK_ERROR_SURFACE_LOST_KHR:
	    strcat_s(msgBuffer, " VK_ERROR_SURFACE_LOST_KHR\n");
	    error = true;
	    break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
	    strcat_s(msgBuffer, " VK_ERROR_NATIVE_WINDOW_IN_USE_KHR\n");
	    error = true;
	    break;
	case VK_SUBOPTIMAL_KHR:
	    break;
	case VK_ERROR_OUT_OF_DATE_KHR:
	    strcat_s(msgBuffer, " VK_ERROR_OUT_OF_DATE_KHR\n");
	    error = true;
	    break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
	    strcat_s(msgBuffer, " VK_ERROR_INCOMPATIBLE_DISPLAY_KHR\n");
	    error = true;
	    break;
	case VK_ERROR_VALIDATION_FAILED_EXT:
	    strcat_s(msgBuffer, " VK_ERROR_VALIDATION_FAILED_EXT\n");
	    error = true;
	    break;
	case VK_ERROR_INVALID_SHADER_NV:
	    strcat_s(msgBuffer, " VK_ERROR_INVALID_SHADER_NV\n");
	    error = true;
	    break;
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
	    strcat_s(msgBuffer, " VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT\n");
	    error = true;
	    break;
	case VK_ERROR_NOT_PERMITTED_EXT:
	    strcat_s(msgBuffer, " VK_ERROR_NOT_PERMITTED_EXT\n");
	    error = true;
	    break;
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
	    strcat_s(msgBuffer, " VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT\n");
	    error = true;
	    break;
	default:
	    break;
    }

    if(error)
    {
	LOG_ERROR("{0}", msgBuffer);
    }

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