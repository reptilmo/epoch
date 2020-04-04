
#include "../../../String/StringUtilities.h"
#include "../../../Platform/FileHelper.h"

#include "VulkanUtilities.h"
#include "VulkanDevice.h"
#include "VulkanShader.h"

namespace Epoch {
    VulkanShader::VulkanShader( VulkanDevice* device, const char* name, ShaderType type ) {
        _device = device;
        _name = name;

        const char* shaderType;
        VkShaderStageFlagBits stage;
        switch( type ) {
        default:
        case ShaderType::Vertex:
            shaderType = "vert";
            stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ShaderType::Fragment:
            shaderType = "frag";
            stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case ShaderType::Geometry:
            shaderType = "geom";
            stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            break;
        case ShaderType::Compute:
            shaderType = "comp";
            stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;
        }

        // TODO: Should probably have this path be configurable.
        const char* fileName = StringUtilities::Format( "shaders/%s.%s.spv", name, shaderType );
        VkShaderModuleCreateInfo shaderCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        shaderCreateInfo.pCode = (U32*)FileHelper::ReadFileBinaryToArray( fileName, &shaderCreateInfo.codeSize );
        VK_CHECK( vkCreateShaderModule( _device->LogicalDevice, &shaderCreateInfo, nullptr, &_handle ) );

        // Create shader stage info.
        _shaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        _shaderStageCreateInfo.stage = stage;
        _shaderStageCreateInfo.module = _handle;
        _shaderStageCreateInfo.pName = "main";
    }

    VulkanShader::~VulkanShader() {
        if( _handle ) {
            vkDestroyShaderModule( _device->LogicalDevice, _handle, nullptr );
            _handle = nullptr;
        }

        _device = nullptr;
    }
}