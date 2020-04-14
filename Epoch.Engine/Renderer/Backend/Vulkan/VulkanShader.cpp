
#include "../../../String/StringUtilities.h"
#include "../../../Platform/FileHelper.h"
#include "../../../Events/Event.h"
#include "../../Material.h"
#include "../../../Resources/ITexture.h"

#include "VulkanRenderPassManager.h"
#include "VulkanBuffer.h"
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
        for( U32 i = 0; i < _imageCount; ++i ) {
            if( _globalUniformBuffers[i] ) {
                delete _globalUniformBuffers[i];
                _globalUniformBuffers[i] = nullptr;
            }

            if( _objectUniformBuffers[i] ) {
                delete _objectUniformBuffers[i];
                _objectUniformBuffers[i] = nullptr;
            }
        }
        _globalUniformBuffers.clear();
        _objectUniformBuffers.clear();
        _globalUbos.clear();
        _objectUbos.clear();
        
        destroyPipeline();

        for( U32 i = 0; i < _imageCount; ++i ) {
            if( _textureSamplers[i] ) {
                delete _textureSamplers[i];
                _textureSamplers[i] = nullptr;
            }
        }
        _textureSamplers.clear();

        for( U32 i = 0; i < _imageCount; ++i ) {
            if( _globalDescriptorPools[i] ) {
                vkDestroyDescriptorPool( _device->LogicalDevice, _globalDescriptorPools[i], nullptr );
                _globalDescriptorPools[i] = nullptr;
            }

            if( _objectDescriptorPools[i] ) {
                vkDestroyDescriptorPool( _device->LogicalDevice, _objectDescriptorPools[i], nullptr );
                _objectDescriptorPools[i] = nullptr;
            }
        }
        _globalDescriptorPools.clear();
        _objectDescriptorPools.clear();

        if( _globalDescriptorSetLayout ) {
            vkDestroyDescriptorSetLayout( _device->LogicalDevice, _globalDescriptorSetLayout, nullptr );
            _globalDescriptorSetLayout = nullptr;
        }

        if( _objectDescriptorSetLayout ) {
            vkDestroyDescriptorSetLayout( _device->LogicalDevice, _objectDescriptorSetLayout, nullptr );
            _objectDescriptorSetLayout = nullptr;
        }

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


    void VulkanShader::SetGlobalUniform( ICommandBuffer* commandBuffer, const GlobalUniformObject& uniform, const U32 frameIndex ) {
        // may need to:
        // bind pipeline

        // need to:
        // alloc desc
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = _globalDescriptorPools[frameIndex];
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &_globalDescriptorSetLayout;

        VK_CHECK( vkAllocateDescriptorSets( _device->LogicalDevice, &allocInfo, &_globalDescriptorSets[frameIndex] ) );

        // Configure the descriptors for the given index.
        U32 range = sizeof( GlobalUniformObject ); // nvidia cards require 256-byte alignment
        U64 offset = 0;// range* (U64)frameIndex;

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = _globalUniformBuffers[frameIndex]->GetHandle();// static_cast<VulkanUniformBuffer*>( uniformBuffer )->GetHandle();
        bufferInfo.offset = offset;
        bufferInfo.range = range;

        // Perform the ubo update.
        _globalUbos[frameIndex].Projection = uniform.Projection;
        _globalUbos[frameIndex].View = uniform.View;

        // Copy
        void* data = _globalUniformBuffers[frameIndex]->GetInternal()->LockMemory( offset, range, 0 );
        TMemory::Memcpy( data, &_globalUbos[frameIndex], sizeof( GlobalUniformObject ) );
        _globalUniformBuffers[frameIndex]->GetInternal()->UnlockMemory();

        VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descriptorWrite.dstSet = _globalDescriptorSets[frameIndex];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.pNext = nullptr;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets( _device->LogicalDevice, 1, &descriptorWrite, 0, nullptr );
    }

    void VulkanShader::SetModel( const Matrix4x4& model ) {
        _model = model;
    }

    void VulkanShader::ResetDescriptors( const U32 frameIndex ) {
        VK_CHECK( vkResetDescriptorPool( _device->LogicalDevice, _globalDescriptorPools[frameIndex], 0 ) );
        VK_CHECK( vkResetDescriptorPool( _device->LogicalDevice, _objectDescriptorPools[frameIndex], 0 ) );
    }

    void VulkanShader::BindPipeline( ICommandBuffer* commandBuffer ) {
        _graphicsPipeline->Bind( static_cast<VulkanCommandBuffer*>( commandBuffer ) );
    }

    void VulkanShader::UpdateDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex, BaseMaterial* material ) {

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = _objectDescriptorPools[frameIndex];
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &_objectDescriptorSetLayout;

        VK_CHECK( vkAllocateDescriptorSets( _device->LogicalDevice, &allocInfo, &_objectDescriptorSets[frameIndex][objectIndex] ) );
    }

    void VulkanShader::BindDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex ) {
        //vkCmdBindDescriptorSets( static_cast<VulkanCommandBuffer*>( commandBuffer )->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline->GetLayout(), 0, 1, &_objectDescriptorSets[frameIndex][objectIndex], 0, nullptr );
        
        VkDescriptorSet descriptorSets[2];
        descriptorSets[0] = _globalDescriptorSets[frameIndex];
        descriptorSets[1] = _objectDescriptorSets[frameIndex][objectIndex]; 

        vkCmdBindDescriptorSets( static_cast<VulkanCommandBuffer*>( commandBuffer )->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline->GetLayout(), 
            0, 2, descriptorSets, 0, nullptr );

        /*vkCmdBindDescriptorSets( static_cast<VulkanCommandBuffer*>( commandBuffer )->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline->GetLayout(),
            0, 1, &descriptorSets[0], 0, nullptr );

        vkCmdBindDescriptorSets( static_cast<VulkanCommandBuffer*>( commandBuffer )->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline->GetLayout(),
            0, 1, &descriptorSets[1], 0, nullptr );*/
    }

    void VulkanShader::intialize() {
        createDescriptorSetLayout();
        createDescriptorPools();
        createTextureSamplers();
        createPipeline( _device->FramebufferSize );
        createUniformBuffers();

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

    void VulkanUnlitShader::UpdateDescriptor( ICommandBuffer* commandBuffer, const U32 frameIndex, const U32 objectIndex, BaseMaterial* material ) {

        VulkanShader::UpdateDescriptor( commandBuffer, frameIndex, objectIndex, material );

        // Configure the descriptors for the given index.
        U32 range = sizeof( UnlitUniformObject ); // nvidia cards require 256-byte alignment
        U64 offset = range * (U64)objectIndex;

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = _objectUniformBuffers[frameIndex]->GetHandle();// static_cast<VulkanUniformBuffer*>( uniformBuffer )->GetHandle();
        bufferInfo.offset = offset;
        bufferInfo.range = range;

        // Perform the ubo update.
        _objectUbos[frameIndex].Model = _model;        

        // Copy
        void* data = _objectUniformBuffers[frameIndex]->GetInternal()->LockMemory( offset, range, 0 );
        TMemory::Memcpy( data, &_objectUbos[frameIndex], sizeof( UnlitUniformObject ) );
        _objectUniformBuffers[frameIndex]->GetInternal()->UnlockMemory();

        // Map images to samplers
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
        descriptorWrites[0].dstSet = _objectDescriptorSets[frameIndex][objectIndex];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].pNext = nullptr;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        // Diffuse sampler.
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _objectDescriptorSets[frameIndex][objectIndex];
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

        // GLOBAL LAYOUT
        VkDescriptorSetLayoutBinding globalUboLayoutBinding = {};
        globalUboLayoutBinding.binding = 0;
        globalUboLayoutBinding.descriptorCount = 1;
        globalUboLayoutBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        globalUboLayoutBinding.pImmutableSamplers = nullptr;
        globalUboLayoutBinding.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo globalLayoutInfo = {};
        globalLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        globalLayoutInfo.bindingCount = 1;
        globalLayoutInfo.pBindings = &globalUboLayoutBinding;
        VK_CHECK( vkCreateDescriptorSetLayout( _device->LogicalDevice, &globalLayoutInfo, nullptr, &_globalDescriptorSetLayout ) );

        // PER-OBJECT LAYOUT -------------------------------------------------------

        // Layout specific to this shader.
        VkDescriptorSetLayoutBinding objectUboLayoutBinding = {};
        objectUboLayoutBinding.binding = 0;
        objectUboLayoutBinding.descriptorCount = 1;
        objectUboLayoutBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        objectUboLayoutBinding.pImmutableSamplers = nullptr;
        objectUboLayoutBinding.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        const U32 bindingCount = 2;
        VkDescriptorSetLayoutBinding bindings[bindingCount] = {
            objectUboLayoutBinding,
            samplerLayoutBinding
        };

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = bindingCount;
        layoutInfo.pBindings = bindings;
        VK_CHECK( vkCreateDescriptorSetLayout( _device->LogicalDevice, &layoutInfo, nullptr, &_objectDescriptorSetLayout ) );
    }

    void VulkanUnlitShader::createDescriptorPools() {

        // GLOBAL
        // The first section will be used for uniform buffers
        VkDescriptorPoolSize poolSize;
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = _imageCount;

        VkDescriptorPoolCreateInfo globalPoolInfo = {};
        globalPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        globalPoolInfo.poolSizeCount = 1;
        globalPoolInfo.pPoolSizes = &poolSize;
        globalPoolInfo.maxSets = _imageCount;

        // Create a pool per frame (double/triple).
        _globalDescriptorPools.resize( _imageCount );
        _globalDescriptorSets.resize( _imageCount );
        for( U32 i = 0; i < _imageCount; ++i ) {
            VK_CHECK( vkCreateDescriptorPool( _device->LogicalDevice, &globalPoolInfo, nullptr, &_globalDescriptorPools[i] ) );
        }

        // PER-OBJECT
        const U32 poolSizeCount = 2;
        VkDescriptorPoolSize poolSizes[poolSizeCount];

        // The first section will be used for uniform buffers
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = VULKAN_MAX_UNIFORM_BUFFERS;

        // The second section will be used for image samplers.
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = VULKAN_MAX_IMAGE_SAMPLERS;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizeCount;
        poolInfo.pPoolSizes = poolSizes;
        poolInfo.maxSets = VULKAN_MAX_DESC_SETS;

        // Create a pool per frame (double/triple).
        _objectDescriptorPools.resize( _imageCount );
        _objectDescriptorSets.resize( _imageCount );
        for( U32 i = 0; i < _imageCount; ++i ) {
            _objectDescriptorSets[i].resize( VULKAN_MAX_DESC_SETS );
            VK_CHECK( vkCreateDescriptorPool( _device->LogicalDevice, &poolInfo, nullptr, &_objectDescriptorPools[i] ) );
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
        info.DescriptorSetLayouts.push_back( _globalDescriptorSetLayout );
        info.DescriptorSetLayouts.push_back( _objectDescriptorSetLayout );
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

    void VulkanUnlitShader::createUniformBuffers() {

        // Per-object UBOs
        _objectUbos.resize( _imageCount );
        _objectUniformBuffers.resize( _imageCount );
        for( U64 i = 0; i < _imageCount; ++i ) {
            _objectUniformBuffers[i] = new VulkanBuffer<Epoch::UnlitUniformObject>( _device, VulkanBufferType::UNIFORM );
            _objectUniformBuffers[i]->Allocate( sizeof( UnlitUniformObject ) * VULKAN_MAX_UNIFORM_BUFFERS );
        }

        // Global UBOs
        _globalUbos.resize( _imageCount );
        _globalUniformBuffers.resize( _imageCount );
        for( U64 i = 0; i < _imageCount; ++i ) {
            _globalUniformBuffers[i] = new VulkanBuffer<GlobalUniformObject>( _device, VulkanBufferType::UNIFORM );
            _globalUniformBuffers[i]->Allocate( sizeof( GlobalUniformObject ) * _imageCount );
        }
    }
}