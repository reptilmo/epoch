
#include "../../../Logger.h"
#include "../../../Math/Vector3.h"
#include "../../Vertex3D.h"

#include "VulkanUtilities.h"
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandBuffer.h"
#include "VulkanPipeline.h"

namespace Epoch {
    VulkanPipeline::VulkanPipeline( VulkanDevice* device ) {
        _device = device;
    }

    VulkanPipeline:: ~VulkanPipeline() {
        if( _handle ) {
            vkDestroyPipeline( _device->LogicalDevice, _handle, nullptr );
            _handle = nullptr;
        }

        if( _layout ) {
            vkDestroyPipelineLayout( _device->LogicalDevice, _layout, nullptr );
            _layout = nullptr;
        }

        _device = nullptr;
    }


    VulkanGraphicsPipeline::VulkanGraphicsPipeline( VulkanDevice* device, const PipelineInfo& info ) : VulkanPipeline( device ) {
        createLayout( info );
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
        VulkanPipeline::~VulkanPipeline();
    }

    void VulkanGraphicsPipeline::Bind( VulkanCommandBuffer* commandBuffer ) {
        if( _handle ) {
            vkCmdBindPipeline( commandBuffer->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, _handle );
        } else {
            Logger::Fatal( "Attempted to bind a pipeline which does not have a handle." );
        }
    }

    void VulkanGraphicsPipeline::createLayout( const PipelineInfo& info ) {

        // Viewport
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = (F32)info.Extent.height;
        viewport.width = (F32)info.Extent.width;
        viewport.height = -(F32)info.Extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Scissor
        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = { info.Extent.width, info.Extent.height };

        // Viewport state
        VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizerCreateInfo.depthClampEnable = VK_FALSE;
        rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizerCreateInfo.lineWidth = 1.0f;
        rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;// VK_CULL_MODE_BACK_BIT;
        rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //VK_FRONT_FACE_COUNTER_CLOCKWISE // VK_FRONT_FACE_CLOCKWISE
        rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
        rasterizerCreateInfo.depthBiasClamp = 0.0f;
        rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

        // Multisampling.
        VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
        multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisamplingCreateInfo.minSampleShading = 1.0f;
        multisamplingCreateInfo.pSampleMask = nullptr;
        multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

        // Depth and stencil testing.
        VkPipelineDepthStencilStateCreateInfo depthStencil = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
        colorBlendAttachmentState.blendEnable = VK_FALSE;
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
        colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

        // Dynamic state
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicStateCreateInfo.dynamicStateCount = 2;
        dynamicStateCreateInfo.pDynamicStates = dynamicStates;

        // Vertex input
        VkVertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0; // Binding index
        bindingDescription.stride = sizeof( Vertex3D );
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Move to next data entry for each vertex.

        U32 offset = 0;
        VkVertexInputAttributeDescription attributeDescriptions[4];
        attributeDescriptions[0].binding = 0; // binding index - should match binding desc
        attributeDescriptions[0].location = 0; // attrib location
        attributeDescriptions[0].format = VkFormat::VK_FORMAT_R32G32B32_SFLOAT; // 3x32-bit floats
        attributeDescriptions[0].offset = 0;
        offset += sizeof( Vector3 );

        attributeDescriptions[1].binding = 0; // binding index - should match binding desc
        attributeDescriptions[1].location = 1; // attrib location
        attributeDescriptions[1].format = VkFormat::VK_FORMAT_R32G32B32_SFLOAT; // 3x32-bit floats
        attributeDescriptions[1].offset = offset;
        offset += sizeof( Vector3 );

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offset;
        offset += sizeof( Vector2 );

        // Color
        attributeDescriptions[3].binding = 0; // binding index - should match binding desc
        attributeDescriptions[3].location = 3; // attrib location
        attributeDescriptions[3].format = VkFormat::VK_FORMAT_R32G32B32_SFLOAT; // 3x32-bit floats
        attributeDescriptions[3].offset = offset;
        offset += sizeof( Vector3 );

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = 4;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

        // Input assembly 
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.setLayoutCount = (U32)info.DescriptorSetLayouts.size();
        pipelineLayoutCreateInfo.pSetLayouts = info.DescriptorSetLayouts.data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        VK_CHECK( vkCreatePipelineLayout( _device->LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &_layout ) );

        // Pipeline create
        VkGraphicsPipelineCreateInfo pipelineCreateInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipelineCreateInfo.stageCount = (U32)info.ShaderStages.size();
        pipelineCreateInfo.pStages = info.ShaderStages.data();
        pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssembly;;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
        pipelineCreateInfo.pDepthStencilState = &depthStencil;
        pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        pipelineCreateInfo.pDynamicState = nullptr;

        pipelineCreateInfo.layout = _layout;

        // TODO: Get once and save
        pipelineCreateInfo.renderPass = info.Renderpass->GetHandle();
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        VK_CHECK( vkCreateGraphicsPipelines( _device->LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_handle ) );

        Logger::Log( "Graphics pipeline created!" );
    }
}