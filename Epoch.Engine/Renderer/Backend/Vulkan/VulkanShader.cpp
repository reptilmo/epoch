
#include "../../../String/StringUtilities.h"
#include "../../../Platform/FileHelper.h"
#include "../../../Events/Event.h"
#include "../../Material.h"
#include "../../../Resources/ITexture.h"

#include "VulkanRenderPassManager.h"
#include "VulkanUniformBuffer.h"
#include "VulkanImage.h"
#include "VulkanTextureSampler.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"

namespace Epoch {
    VulkanShaderModule::VulkanShaderModule( VulkanDevice* device, const char* name, ShaderType type ) {
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
        _shaderStageCreateInfo = { VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        _shaderStageCreateInfo.stage = stage;
        _shaderStageCreateInfo.module = _handle;
        _shaderStageCreateInfo.pName = "main";
    }

    VulkanShaderModule::~VulkanShaderModule() {
        if( _handle ) {
            vkDestroyShaderModule( _device->LogicalDevice, _handle, nullptr );
            _handle = nullptr;
        }

        _device = nullptr;
    }

    VulkanShader::VulkanShader( VulkanDevice* device, const char* name, const U32 imageCount, const TString& renderPassName,
        const bool hasVertex, const bool hasFragment, const bool hasGeometry, const bool hasCompute ) {

        _device = device;
        _imageCount = imageCount;
        _renderPassName = renderPassName;

        if( hasVertex ) {
            _vertexModule = new VulkanShaderModule( device, name, ShaderType::Vertex );
        }

        if( hasFragment ) {
            _fragmentModule = new VulkanShaderModule( device, name, ShaderType::Fragment );
        }

        if( hasGeometry ) {
            _geometryModule = new VulkanShaderModule( device, name, ShaderType::Geometry );
        }

        if( hasCompute ) {
            _computeModule = new VulkanShaderModule( device, name, ShaderType::Compute );
        }
    }

    VulkanShader::~VulkanShader() {
        if( _vertexModule ) {
            delete _vertexModule;
            _vertexModule = nullptr;
        }

        if( _fragmentModule ) {
            delete _fragmentModule;
            _fragmentModule = nullptr;
        }

        if( _geometryModule ) {
            delete _geometryModule;
            _geometryModule = nullptr;
        }

        if( _computeModule ) {
            delete _computeModule;
            _computeModule = nullptr;
        }

        for( U32 i = 0; i < _imageCount; ++i ) {
            if( _descriptorPools[i] ) {
                vkDestroyDescriptorPool( _device->LogicalDevice, _descriptorPools[i], nullptr );
                _descriptorPools[i] = nullptr;
            }
        }
        _descriptorPools.clear();

        for( U32 i = 0; i < _imageCount; ++i ) {
            delete _textureSamplers[0];
            _textureSamplers[0] = nullptr;
        }

        if( _descriptorSetLayout ) {
            vkDestroyDescriptorSetLayout( _device->LogicalDevice, _descriptorSetLayout, nullptr );
            _descriptorSetLayout = nullptr;
        }

        destroyPipeline();
    }

    void VulkanShader::OnEvent( const Event* event ) {
        switch( event->Type ) {
        case EventType::WINDOW_RESIZED:
            destroyPipeline();
            createPipeline( _device->FramebufferSize );
            break;
        default:
            break;
        }
    }

    void VulkanShader::ResetDescriptors( const U32 frameIndex ) {
        VK_CHECK( vkResetDescriptorPool( _device->LogicalDevice, _descriptorPools[frameIndex], 0 ) );
    }

    void VulkanShader::BindPipeline( ICommandBuffer* commandBuffer ) {
        _graphicsPipeline->Bind( static_cast<VulkanCommandBuffer*>( commandBuffer ) );
    }

    void VulkanShader::UpdateDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex, IUniformBuffer* uniformBuffer, BaseMaterial* material ) {

        // Fill an array with pointers to the descriptor set layout.
        std::vector<VkDescriptorSetLayout> layouts( _imageCount, _descriptorSetLayout );

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPools[frameIndex];
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &_descriptorSetLayout;

        VK_CHECK( vkAllocateDescriptorSets( _device->LogicalDevice, &allocInfo, &_descriptorSets[frameIndex][objectIndex] ) );
    }

    void VulkanShader::BindDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex ) {
        vkCmdBindDescriptorSets( static_cast<VulkanCommandBuffer*>( commandBuffer )->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline->GetLayout(), 0, 1, &_descriptorSets[frameIndex][objectIndex], 0, nullptr );
    }

    void VulkanShader::intialize() {
        createDescriptorSetLayout();
        createDescriptorPools();
        createTextureSamplers();
        createPipeline( _device->FramebufferSize );

        // Listen for resize events.
        Event::Listen( EventType::WINDOW_RESIZED, this );
    }

    void VulkanShader::destroyPipeline() {
        if( _graphicsPipeline ) {
            delete _graphicsPipeline;
            _graphicsPipeline = nullptr;
        }
    }

    // ////////////////////////////////////////////////////////////////////////////////////////
    // ///////////////////////////////////// Unlit Shader /////////////////////////////////////
    // ////////////////////////////////////////////////////////////////////////////////////////

    VulkanUnlitShader::VulkanUnlitShader( VulkanDevice* device, const U32 imageCount, const TString& renderPassName ) :
        VulkanShader( device, BUILTIN_SHADER_NAME_UNLIT, imageCount, renderPassName, true, true, false, false ) {

        intialize();
    }

    VulkanUnlitShader::~VulkanUnlitShader() {
        VulkanShader::~VulkanShader();
    }

    void VulkanUnlitShader::UpdateDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex, IUniformBuffer* uniformBuffer, BaseMaterial* material ) {

        VulkanShader::UpdateDescriptor( commandBuffer, frameIndex, objectIndex, uniformBuffer, material );

        // Configure the descriptors for the given index.
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = static_cast<VulkanUniformBuffer*>( uniformBuffer )->GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof( StandardUniformBufferObject );

        UnlitMaterial* castMaterial = static_cast<UnlitMaterial*>( material );
        VulkanImage* diffuseImage = static_cast<VulkanImage*>( castMaterial->DiffuseMap->GetImage() );
        VkDescriptorImageInfo imageInfo = {};
        if( diffuseImage != nullptr ) {
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = diffuseImage->GetView();
            imageInfo.sampler = _textureSamplers[frameIndex]->GetHandle();
        }

        std::vector<VkWriteDescriptorSet> descriptorWrites( 2 );
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = _descriptorSets[frameIndex][objectIndex];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].pNext = nullptr;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        // Diffuse sampler.
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _descriptorSets[frameIndex][objectIndex];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].pNext = nullptr;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        if( diffuseImage != nullptr ) {
            descriptorWrites[1].pImageInfo = &imageInfo;
        } else {
            descriptorWrites[1].pImageInfo = nullptr;
        }

        vkUpdateDescriptorSets( _device->LogicalDevice, 2, descriptorWrites.data(), 0, nullptr );
    }

    void VulkanUnlitShader::createDescriptorSetLayout() {

        // Layout specific to this shader.
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        const U32 bindingCount = 2;
        VkDescriptorSetLayoutBinding bindings[bindingCount] = {
            uboLayoutBinding,
            samplerLayoutBinding
        };

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = bindingCount;
        layoutInfo.pBindings = bindings;

        VK_CHECK( vkCreateDescriptorSetLayout( _device->LogicalDevice, &layoutInfo, nullptr, &_descriptorSetLayout ) );
    }

    void VulkanUnlitShader::createDescriptorPools() {
        const U32 poolSizeCount = 2;
        VkDescriptorPoolSize poolSizes[poolSizeCount];
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = VULKAN_MAX_UNIFORM_BUFFERS;
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = VULKAN_MAX_IMAGE_SAMPLERS;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizeCount;
        poolInfo.pPoolSizes = poolSizes;
        poolInfo.maxSets = VULKAN_MAX_DESC_SETS;

        // Create a pool per frame (double/triple).
        _descriptorPools.resize( _imageCount );
        _descriptorSets.resize( _imageCount );
        for( U32 i = 0; i < _imageCount; ++i ) {
            _descriptorSets[i].resize( VULKAN_MAX_DESC_SETS );
            VK_CHECK( vkCreateDescriptorPool( _device->LogicalDevice, &poolInfo, nullptr, &_descriptorPools[i] ) );
        }
    }

    void VulkanUnlitShader::createTextureSamplers() {
        for( U32 i = 0; i < _imageCount; ++i ) {
            _textureSamplers.push_back( new VulkanTextureSampler( _device ) );
        }
    }

    void VulkanUnlitShader::createPipeline( const Extent2D& extent ) {
        PipelineInfo info;
        info.Extent = { (U32)extent.Width, (U32)extent.Height };
        info.Renderpass = VulkanRenderPassManager::GetRenderPass( "RenderPass.Default" );
        info.DescriptorSetLayouts.push_back( _descriptorSetLayout );
        if( HasVertexStage() ) {
            info.ShaderStages.push_back( _vertexModule->GetShaderStageCreateInfo() );
        }

        if( HasFragmentStage() ) {
            info.ShaderStages.push_back( _fragmentModule->GetShaderStageCreateInfo() );
        }

        if( HasGeometryStage() ) {
            info.ShaderStages.push_back( _geometryModule->GetShaderStageCreateInfo() );
        }

        if( HasComputeStage() ) {
            info.ShaderStages.push_back( _computeModule->GetShaderStageCreateInfo() );
        }

        _graphicsPipeline = new VulkanGraphicsPipeline( _device, info );
    }
}