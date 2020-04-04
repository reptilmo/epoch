#pragma once

#include <vulkan/vulkan.h>

namespace Epoch {

    class VulkanDevice;

    enum ShaderType {
        Vertex,
        Fragment,
        Compute,
        Geometry
    };

    class VulkanShader {
    public:
        VulkanShader( VulkanDevice* device, const char* name, ShaderType type );
        ~VulkanShader();

        VkShaderModule GetHandle() { return _handle; }
        VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() { return _shaderStageCreateInfo; }
    private:
        const char* _name;
        VulkanDevice* _device;
        VkShaderModule _handle;
        VkPipelineShaderStageCreateInfo _shaderStageCreateInfo;
    };
}