#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Epoch {

    class VulkanDevice;
    class VulkanCommandBuffer;
    class VulkanRenderPass;

    struct PipelineInfo {
        VkExtent2D Extent = { 0, 0 };
        std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;
        VulkanRenderPass* Renderpass = nullptr;
        std::vector<VkPipelineShaderStageCreateInfo> ShaderStages;
    };

    class VulkanPipeline {
    public:
        VulkanPipeline( VulkanDevice* device );
        virtual ~VulkanPipeline();

        VkPipeline GetHandle() { return _handle; }
        VkPipelineLayout GetLayout() { return _layout; }

    protected:
        VkPipeline _handle = nullptr;
        VkPipelineLayout _layout = nullptr;
        VulkanDevice* _device = nullptr;
    };

    class VulkanGraphicsPipeline : public VulkanPipeline {
    public:
        VulkanGraphicsPipeline( VulkanDevice* device, const PipelineInfo& info );
        ~VulkanGraphicsPipeline();

        void Bind( VulkanCommandBuffer* commandBuffer );

    private:
        void createLayout( const PipelineInfo& info );
    };
}