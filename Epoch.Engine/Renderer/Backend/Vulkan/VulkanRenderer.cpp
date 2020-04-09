
#include <vector>
#include <fstream>

// TODO: temp
#include <chrono>

#include "../../../Platform/Platform.h"
#include "../../../Logger.h"
#include "../../../Defines.h"
#include "../../../Memory/Memory.h"
#include "../../../Math/TMath.h"
#include "../../../Math/Rotator.h"
#include "../../../Math/Matrix4x4.h"
#include "../../../Math/Vector3.h"
#include "../../../Math/Quaternion.h"
#include "../../../String/TString.h"

#include "../../StandardUniformBufferObject.h"
#include "../../../Resources/ITexture.h"
#include "../../MeshData.h"
#include "../../Material.h"
#include "../../IShader.h"

#include "VulkanUtilities.h"
#include "VulkanImage.h"
#include "VulkanTexture.h"
#include "VulkanInternalBuffer.h"
#include "VulkanVertex3DBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanDebugger.h"
#include "VulkanDevice.h"
#include "VulkanRenderPassManager.h"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanPipeline.h"
#include "VulkanTextureSampler.h"
#include "VulkanQueue.h"
#include "VulkanShader.h"

#include "VulkanRenderer.h"

namespace Epoch {

    VulkanRenderer::VulkanRenderer( Platform* platform ) {
        _platform = platform;
        Logger::Trace( "Creating Vulkan renderer..." );

    }

    VulkanRenderer::~VulkanRenderer() {
        if( _instance ) {

            // Make sure things are destroyed.
            Destroy();
        }
    }

    const bool VulkanRenderer::Initialize() {
        Logger::Trace( "Initializing Vulkan renderer..." );

        createInstance();

        // Create debugger
        _debugger = new VulkanDebugger( _instance, VulkanDebuggerLevel::ERROR | VulkanDebuggerLevel::WARNING );

        // Create the surface
        _platform->CreateSurface( _instance, &_surface );

        // Create VulkanDevice
        _device = new VulkanDevice( _instance, _requiredValidationLayers, _surface );

        // HACK: This is here until the platform layer is removed.
        _device->FramebufferSize = _platform->GetFramebufferExtent();

        // Shader creation.
        //_unlitShader = new VulkanUnlitShader( _device, "main", true, true, false, false );

        // Create swapchain
        Extent2D extent = _platform->GetFramebufferExtent();
        _swapchain = new VulkanSwapchain( _device, _surface, _platform->GetWindow(), extent.Width, extent.Height );

        // Listen for resize events.
        Event::Listen( EventType::WINDOW_RESIZED, this );


        // Create renderpass.
        createRenderPass();
        _swapchain->RegenerateFramebuffers();

        // Built-in shader creation.
        _unlitShader = new VulkanUnlitShader( _device, _swapchain->GetSwapchainImageCount(), "RenderPass.Default" );

        //createDescriptorSetLayout();

        //createGraphicsPipeline();

        //// Create samplers - one per in-flight frame. TODO: should probably be held by the shader.
        //_textureSamplers[0] = new VulkanTextureSampler( _device );
        //_textureSamplers[1] = new VulkanTextureSampler( _device );

        // TODO: load model
        createBuffers();

        // End asset loading.

        // Create UBOs
        createUniformBuffers();
        //createDescriptorPool();
        //createDescriptorSets();
        U64 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            //updateDescriptorSet( i, (VulkanImage*)_textures[_currentTextureIndex]->GetImage(), _textureSamplers[_currentTextureIndex] );
        }

        createCommandBuffers();
        createSyncObjects();



        return true;
    }

    void VulkanRenderer::Destroy() {
        VK_CHECK( vkDeviceWaitIdle( _device->LogicalDevice ) );

        // sync objects
        for( U8 i = 0; i < _swapchain->MaxFramesInFlight; ++i ) {
            delete _imageAvailableSemaphores[i];
            delete _renderCompleteSemaphores[i];

            delete _inFlightFences[i];
            _inFlightFences[i] = nullptr;
            _inFlightImageFences[i] = nullptr;
        }
        _inFlightFences.clear();
        _inFlightImageFences.clear();
        _imageAvailableSemaphores.clear();
        _renderCompleteSemaphores.clear();

        U64 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            _device->CommandPool->FreeCommandBuffer( _commandBuffers[i] );
        }
        _commandBuffers.clear();

        //vkDestroyDescriptorPool( _device->LogicalDevice, _descriptorPool, nullptr );

        for( auto buffer : _uniformBuffers ) {
            delete buffer;
        }
        _uniformBuffers.clear();

        if( _indexBuffer ) {
            delete _indexBuffer;
            _indexBuffer = nullptr;
        }

        if( _vertexBuffer ) {
            delete _vertexBuffer;
            _vertexBuffer = nullptr;
        }

        // TEMP
        /*if( _textureSamplers[0] ) {
            delete _textureSamplers[0];
            _textureSamplers[0] = nullptr;
        }

        if( _textureSamplers[1] ) {
            delete _textureSamplers[1];
            _textureSamplers[1] = nullptr;
        }*/

        /*if( _textures[0] ) {
            delete _textures[0];
            _textures[0] = nullptr;
        }*/

        /*if( _textures[1] ) {
            delete _textures[1];
            _textures[1] = nullptr;
        }*/

        /*if( _graphicsPipeline ) {
            delete _graphicsPipeline;
            _graphicsPipeline = nullptr;
        }*/

        //         vkDestroyDescriptorSetLayout( _device->LogicalDevice, _descriptorSetLayout, nullptr );
        VulkanRenderPassManager::DestroyRenderPass( _device, "RenderPass.Default" );

        if( _swapchain ) {
            delete _swapchain;
            _swapchain = nullptr;
        }

        if( _unlitShader ) {
            delete _unlitShader;
            _unlitShader = nullptr;
        }

        vkDestroySurfaceKHR( _instance, _surface, nullptr );

        if( _debugger ) {
            delete _debugger;
            _debugger = nullptr;
        }

        if( _device ) {
            delete _device;
            _device = nullptr;
        }

        if( _instance ) {
            vkDestroyInstance( _instance, nullptr );
            _instance = nullptr;
        }
    }

    const bool VulkanRenderer::PrepareFrame( const F32 deltaTime ) {

        // If currently recreating the swapchain, boot out.
        if( _recreatingSwapchain ) {
            return false;
        }

        if( _framebufferResizeOccurred ) {
            _framebufferResizeOccurred = false;
            recreateSwapchain();
            return false;
        }

        // Wait for the execution of the current frame to complete. The fence being free will allow this to move on.
        if( !_inFlightFences[_swapchain->GetCurrentFrameIndex()]->Wait( U64_MAX ) ) {
            Logger::Warn( "In-flight fence wait failure!" );
        }

        // Acquire next image from the swap chain.
        if( !_swapchain->AcquireNextImageIndex( U64_MAX, _imageAvailableSemaphores[_swapchain->GetCurrentFrameIndex()], nullptr, &_currentImageIndex ) ) {
            return false;
        }



        //// Update the current descriptor set.
        //BaseMaterial* material = _currentRenderList[0]->Material;
        //updateDescriptorSet( _currentImageIndex, (VulkanImage*)_textures[_currentTextureIndex]->GetImage(), _textureSamplers[_currentTextureIndex] );

        // Begin recording.
        _commandBuffers[_currentImageIndex]->Begin();

        // Begin the render pass. TODO: Should probably create these once and reuse.
        VulkanRenderPass* renderPass = VulkanRenderPassManager::GetRenderPass( "RenderPass.Default" );
        RenderPassClearInfo clearInfo;
        clearInfo.Color.Set( 0.0f, 0.0f, 0.2f, 0.0f );
        clearInfo.RenderArea.Set( 0, 0, _swapchain->Extent.width, _swapchain->Extent.height );
        clearInfo.Depth = 1.0f;
        clearInfo.Stencil = 0;
        _commandBuffers[_currentImageIndex]->BeginRenderPass( clearInfo, _swapchain->GetFramebuffer( _currentImageIndex ), renderPass );

        U64 renderListCount = _currentRenderList.size();

        // Bind the buffer to the graphics pipeline
        //_graphicsPipeline->Bind( _commandBuffers[_currentImageIndex] );
        // Get a flat list of all shaders.

        std::vector<IShader*> frameShaders;
        for( U64 i = 0; i < renderListCount; ++i ) {
            bool added = false;
            IShader* shader = _currentRenderList[i]->Material->GetShader();
            for( auto fs : frameShaders ) {
                if( fs == shader ) {
                    added = true;
                    break;
                }
            }
            if( !added ) {
                frameShaders.push_back( shader );
            }
        }

        // Reset the shader descriptors
        for( auto fs : frameShaders ) {
            fs->ResetDescriptors( _currentImageIndex );
        }

        // Draw everything in the render list.        
        for( U64 i = 0; i < renderListCount; ++i ) {
            const MeshRendererReferenceData* ref = _currentRenderList[i];
            const VulkanBufferDataBlock* vertexBlock = _vertexBuffer->GetDataRangeByIndex( ref->VertexHeapIndex );
            const VulkanBufferDataBlock* indexBlock = _indexBuffer->GetDataRangeByIndex( ref->IndexHeapIndex );

            // Update the current descriptor set.
            UnlitMaterial* material = (UnlitMaterial*)ref->Material;
            VulkanShader* shader = (VulkanShader*)material->GetShader();

            // Bind the buffer to the graphics pipeline
            shader->Bind( _commandBuffers[_currentImageIndex], _currentImageIndex, i );

            // Update the descriptor for this object.
            shader->UpdateDescriptor( _commandBuffers[_currentImageIndex], _currentImageIndex, i, _uniformBuffers[_currentImageIndex], material );
            
            // Bind vertex buffers
            VkBuffer vertexBuffers[] = { _vertexBuffer->GetHandle() };
            VkDeviceSize offsets[] = { vertexBlock->Offset }; // was 0
            vkCmdBindVertexBuffers( _commandBuffers[_currentImageIndex]->GetHandle(), 0, 1, vertexBuffers, offsets );

            // Bind index buffer
            vkCmdBindIndexBuffer( _commandBuffers[_currentImageIndex]->GetHandle(), _indexBuffer->GetHandle(), indexBlock->Offset, VK_INDEX_TYPE_UINT32 ); // offset was 0

            // Bind descriptor sets (UBOs and samplers)
            //vkCmdBindDescriptorSets( _commandBuffers[_currentImageIndex]->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline->GetLayout(), 0, 1, &_descriptorSets[_currentImageIndex], 0, nullptr );

            // Make the draw call. TODO: use object properties
            //vkCmdDraw( _commandBuffers[imageIndex], 3, 1, 0, 0 );

            vkCmdDrawIndexed( _commandBuffers[_currentImageIndex]->GetHandle(), (U32)indexBlock->ElementCount, 1, 0, 0, 0 ); // _indexBuffer->GetElementCount()
        }

        // End render pass.
        _commandBuffers[_currentImageIndex]->EndRenderPass();

        // End recording
        _commandBuffers[_currentImageIndex]->End();

        return true;
    }

    const bool VulkanRenderer::Frame( const F32 deltaTime ) {

        // Check if a previous frame is using this image (i.e. its fence is being waited on)
        U8 swapchainFrameIndex = _swapchain->GetCurrentFrameIndex();
        if( _inFlightImageFences[swapchainFrameIndex] != VK_NULL_HANDLE ) {
            _inFlightImageFences[swapchainFrameIndex]->Wait( U64_MAX );
        }

        // Mark the image fence as in-use by this frame.
        _inFlightImageFences[swapchainFrameIndex] = _inFlightFences[swapchainFrameIndex];

        // UBO
        updateUniformBuffers( _currentImageIndex );

        // Reset the fence for use on the next frame.
        _inFlightImageFences[swapchainFrameIndex]->Reset();

        // Ensure that the operation cannot begin until the image is available.
        _commandBuffers[_currentImageIndex]->AddWaitSemaphore( VK_STRUCTURE_TYPE_SUBMIT_INFO, _imageAvailableSemaphores[swapchainFrameIndex] );

        // Submit the queue and wait for the operation to complete.
        VkSemaphore signalSemaphores[] = { _renderCompleteSemaphores[swapchainFrameIndex]->GetHandle() };
        _device->GraphicsQueue->Submit( _commandBuffers[_currentImageIndex], _inFlightFences[swapchainFrameIndex], 1, signalSemaphores, false );

        // Give the image back to the swapchain.
        _swapchain->Present( _device->GraphicsQueue, _device->PresentationQueue, _renderCompleteSemaphores[swapchainFrameIndex], _currentImageIndex );

        // Clear the render list for the next frame.
        _currentRenderList.clear();

        return true;
    }

    const bool VulkanRenderer::UploadMeshData( const MeshUploadData& data, MeshRendererReferenceData* referenceData ) {

        _device->GraphicsQueue->WaitIdle();

        VulkanBufferDataBlock* vertBlock = _vertexBuffer->AllocateData( data.Vertices );
        VulkanBufferDataBlock* indexBlock = _indexBuffer->AllocateData( data.Indices );

        referenceData->VertexHeapIndex = vertBlock->HeapIndex;
        referenceData->IndexHeapIndex = indexBlock->HeapIndex;

        return true;
    }

    void VulkanRenderer::FreeMeshData( const U64 index ) {

    }

    void VulkanRenderer::AddToFrameRenderList( const MeshRendererReferenceData* referenceData ) {
        _currentRenderList.push_back( referenceData );
    }

    void VulkanRenderer::OnEvent( const Event* event ) {
        switch( event->Type ) {
        case EventType::WINDOW_RESIZED:
            //const WindowResizedEvent wre = (const WindowResizedEvent)event;
            _framebufferResizeOccurred = true;
            if( _platform && _device ) {
                _device->FramebufferSize = _platform->GetFramebufferExtent();
            }
            break;
        default:
            break;
        }
    }

    ITexture* VulkanRenderer::GetTexture( const TString& name, const TString& path ) {
        return new VulkanTexture( _device, name, path );
    }

    Extent2D VulkanRenderer::GetFramebufferExtent() {
        return _platform->GetFramebufferExtent();
    }

    IShader* VulkanRenderer::GetBuiltinMaterialShader( const MaterialType type ) {
        switch( type ) {
        default:
        case MaterialType::Unlit:
            return static_cast<IShader*>( _unlitShader );
        }
    }

    void VulkanRenderer::createInstance() {
        VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pApplicationName = "Epoch";
        appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );

        VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceCreateInfo.pApplicationInfo = &appInfo;

        // Extensions
        const char** pfe = nullptr;
        U32 count = 0;
        _platform->GetRequiredExtensions( &count, &pfe );
        std::vector<const char*> platformExtensions;
        for( U32 i = 0; i < count; ++i ) {
            platformExtensions.push_back( pfe[i] );
        }
        platformExtensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );

        instanceCreateInfo.enabledExtensionCount = (U32)platformExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = platformExtensions.data();

        // Validation layers
        _requiredValidationLayers.push_back( "VK_LAYER_KHRONOS_validation" );

        // Get available layers.
        U32 availableLayerCount = 0;
        VK_CHECK( vkEnumerateInstanceLayerProperties( &availableLayerCount, nullptr ) );
        std::vector<VkLayerProperties> availableLayers( availableLayerCount );
        VK_CHECK( vkEnumerateInstanceLayerProperties( &availableLayerCount, availableLayers.data() ) );

        // Verify that all required layers are available.
        bool success = true;
        for( U32 i = 0; i < (U32)_requiredValidationLayers.size(); ++i ) {
            bool found = false;
            for( U32 j = 0; j < availableLayerCount; ++j ) {
                if( strcmp( _requiredValidationLayers[i], availableLayers[j].layerName ) == 0 ) {
                    found = true;
                    break;
                }
            }

            if( !found ) {
                success = false;
                Logger::Fatal( "Required validation layer is missing: %s", _requiredValidationLayers[i] );
                break;
            }
        }

        instanceCreateInfo.enabledLayerCount = (U32)_requiredValidationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = _requiredValidationLayers.data();

        // Create instance
        VK_CHECK( vkCreateInstance( &instanceCreateInfo, nullptr, &_instance ) );
    }

    void VulkanRenderer::createRenderPass() {

        RenderPassData renderPassData;
        renderPassData.Name = "RenderPass.Default";

        RenderTargetOptions colorRenderTargetOptions;
        colorRenderTargetOptions.Format = _swapchain->ImageFormat.format;
        colorRenderTargetOptions.SampleCount = 1;
        colorRenderTargetOptions.LoadOperation = RenderTargetLoadOperation::CLEAR;
        colorRenderTargetOptions.StoreOperation = RenderTargetStoreOperation::STORE;
        colorRenderTargetOptions.StencilLoadOperation = RenderTargetLoadOperation::DONT_CARE;
        colorRenderTargetOptions.StencilStoreOperation = RenderTargetStoreOperation::DONT_CARE;
        colorRenderTargetOptions.InputLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Do not expect any particular layout before render pass starts.
        colorRenderTargetOptions.OutputLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Transitioned to after the render pass        
        renderPassData.ColorRenderTargetOptions.push_back( colorRenderTargetOptions );

        renderPassData.HasDepthRenderTarget = true;
        renderPassData.DepthRenderTargetOptions.Format = _device->DepthFormat;
        renderPassData.DepthRenderTargetOptions.SampleCount = 1;
        renderPassData.DepthRenderTargetOptions.LoadOperation = RenderTargetLoadOperation::CLEAR;
        renderPassData.DepthRenderTargetOptions.StoreOperation = RenderTargetStoreOperation::DONT_CARE;
        renderPassData.DepthRenderTargetOptions.StencilLoadOperation = RenderTargetLoadOperation::DONT_CARE;
        renderPassData.DepthRenderTargetOptions.StencilStoreOperation = RenderTargetStoreOperation::DONT_CARE;
        renderPassData.DepthRenderTargetOptions.InputLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        renderPassData.DepthRenderTargetOptions.OutputLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VulkanRenderPassManager::CreateRenderPass( _device, renderPassData );
    }

    /*void VulkanRenderer::createGraphicsPipeline() {
        PipelineInfo info;
        info.Extent = _swapchain->Extent;
        info.Renderpass = VulkanRenderPassManager::GetRenderPass( "RenderPass.Default" );
        info.DescriptorSetLayouts.push_back( _descriptorSetLayout );
        if( _unlitShader->HasVertexStage() ) {
            info.ShaderStages.push_back( _unlitShader->GetVertexStage()->GetShaderStageCreateInfo() );
        }

        if( _unlitShader->HasFragmentStage() ) {
            info.ShaderStages.push_back( _unlitShader->GetFragmentStage()->GetShaderStageCreateInfo() );
        }

        if( _unlitShader->HasGeometryStage() ) {
            info.ShaderStages.push_back( _unlitShader->GetGeometryStage()->GetShaderStageCreateInfo() );
        }

        if( _unlitShader->HasComputeStage() ) {
            info.ShaderStages.push_back( _unlitShader->GetComputeStage()->GetShaderStageCreateInfo() );
        }

        _graphicsPipeline = new VulkanGraphicsPipeline( _device, info );
    }*/

    void VulkanRenderer::createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof( StandardUniformBufferObject );

        U64 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        _uniformBuffers.resize( swapchainImageCount );

        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            _uniformBuffers[i] = new VulkanUniformBuffer( _device );
            std::vector<StandardUniformBufferObject> udata( 1 );
            _uniformBuffers[i]->SetData( udata );
        }
    }

    void VulkanRenderer::updateUniformBuffers( U32 currentImageIndex ) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        F32 time = std::chrono::duration<F32, std::chrono::seconds::period>( currentTime - startTime ).count();

        VkExtent2D extent = _swapchain->Extent;
        StandardUniformBufferObject ubo = {};
        Rotator rotator;
        rotator.Pitch = time * 90.0f * 0.1f;
        rotator.Roll = 90.0f;
        ubo.Model = Matrix4x4::Identity();
        ubo.Model *= Matrix4x4::Translation( Vector3( 0.0f, 0.0f, 0.0f ) );
        ubo.Model *= rotator.ToQuaternion().ToMatrix4x4();
        ubo.View = Matrix4x4::LookAt( Vector3( 0.0f, 25.0f, 25.0f ), Vector3::Zero(), Vector3::Up() );
        ubo.Projection = Matrix4x4::Perspective( TMath::DegToRad( 90.0f ), (F32)extent.width / (F32)extent.height, 0.1f, 1000.0f );

        // For now, since this happens every frame, just push to the buffer directly.
        void* data = _uniformBuffers[currentImageIndex]->GetInternal()->LockMemory( 0, sizeof( ubo ), 0 );
        TMemory::Memcpy( data, &ubo, sizeof( ubo ) );
        _uniformBuffers[currentImageIndex]->GetInternal()->UnlockMemory();
    }
    /*
    void VulkanRenderer::createDescriptorSetLayout() {
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

        VkDescriptorSetLayoutBinding bindings[2] = {
            uboLayoutBinding,
            samplerLayoutBinding
        };

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = (U32)2;
        layoutInfo.pBindings = bindings;

        VK_CHECK( vkCreateDescriptorSetLayout( _device->LogicalDevice, &layoutInfo, nullptr, &_descriptorSetLayout ) );
    }

    void VulkanRenderer::createDescriptorPool() {
        U32 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        VkDescriptorPoolSize poolSizes[2];
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = swapchainImageCount;
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = swapchainImageCount;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 2;
        poolInfo.pPoolSizes = poolSizes;
        poolInfo.maxSets = swapchainImageCount;

        VK_CHECK( vkCreateDescriptorPool( _device->LogicalDevice, &poolInfo, nullptr, &_descriptorPool ) );
    }
    void VulkanRenderer::createDescriptorSets() {
        U32 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        // Fill an array with pointers to the descriptor set layout.
        std::vector<VkDescriptorSetLayout> layouts( swapchainImageCount, _descriptorSetLayout );

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.descriptorSetCount = (U32)swapchainImageCount;
        allocInfo.pSetLayouts = layouts.data();

        _descriptorSets.resize( swapchainImageCount );
        VK_CHECK( vkAllocateDescriptorSets( _device->LogicalDevice, &allocInfo, _descriptorSets.data() ) );
    }

    void VulkanRenderer::updateDescriptorSet( U64 descriptorSetIndex, VulkanImage* textureImage, VulkanTextureSampler* sampler ) {

        // Configure the descriptors for the given index..
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = _uniformBuffers[descriptorSetIndex]->GetHandle();
        bufferInfo.offset = 0;

        bufferInfo.range = sizeof( StandardUniformBufferObject );

        VkDescriptorImageInfo imageInfo = {};
        if( textureImage != nullptr ) {
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureImage->GetView();
            imageInfo.sampler = sampler->GetHandle();
        }

        std::vector<VkWriteDescriptorSet> descriptorWrites( 2 );
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = _descriptorSets[descriptorSetIndex];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].pNext = nullptr;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // For a UBO
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _descriptorSets[descriptorSetIndex];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].pNext = nullptr;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        if( textureImage != nullptr ) {
            descriptorWrites[1].pImageInfo = &imageInfo;
        } else {
            descriptorWrites[1].pImageInfo = nullptr;
        }

        vkUpdateDescriptorSets( _device->LogicalDevice, 2, descriptorWrites.data(), 0, nullptr );
    }*/

    void VulkanRenderer::createCommandBuffers() {
        U32 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        for( U32 i = 0; i < swapchainImageCount; ++i ) {
            VulkanCommandBuffer* cmdBuf = _device->CommandPool->AllocateCommandBuffer( true );
            _commandBuffers.push_back( cmdBuf );
        }
    }

    void VulkanRenderer::createSyncObjects() {
        U32 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        _imageAvailableSemaphores.resize( _swapchain->MaxFramesInFlight ); // _maxFramesInFlight
        _renderCompleteSemaphores.resize( _swapchain->MaxFramesInFlight );
        _inFlightFences.resize( _swapchain->MaxFramesInFlight );

        for( U8 i = 0; i < _swapchain->MaxFramesInFlight; ++i ) {
            _imageAvailableSemaphores[i] = new VulkanSemaphore( _device );
            _renderCompleteSemaphores[i] = new VulkanSemaphore( _device );

            // Create the fence in a signaled state, indicating that the first frame has already been "rendered".
            // This will prevent the application from waiting indefinitely for the first frame to render since it
            // cannot be rendered until a frame is "rendered" before it.
            _inFlightFences[i] = new VulkanFence( _device, true );
        }

        // In flight fences should not yet exist at this point, so clear the list.
        _inFlightImageFences.resize( swapchainImageCount );
        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            _inFlightImageFences[i] = nullptr;
        }
    }

    void VulkanRenderer::cleanupSwapchain() {


        // Free the command buffers in the pool, but do not destroy the pool.
        U32 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        for( U32 i = 0; i < swapchainImageCount; ++i ) {
            _device->CommandPool->FreeCommandBuffer( _commandBuffers[i] );
        }
        _commandBuffers.clear();

        /*if( _graphicsPipeline ) {
            delete _graphicsPipeline;
            _graphicsPipeline = nullptr;
        }*/


        VulkanRenderPassManager::DestroyRenderPass( _device, "RenderPass.Default" );

        // Destroy UBOs

        // Destroy descriptor pool


    }

    void VulkanRenderer::recreateSwapchain() {
        if( _recreatingSwapchain ) {
            return;
        }
        _recreatingSwapchain = true;
        Extent2D extent = _platform->GetFramebufferExtent();
        while( extent.Width == 0 || extent.Height == 0 ) {
            extent = _platform->GetFramebufferExtent();
            _platform->WaitEvents();
        }

        // Cleanup and recreate swapchain.
        _device->RequerySupport();
        _swapchain->Recreate( extent.Width, extent.Height );

        // Cleanup the old swapchain.
        cleanupSwapchain();

        createRenderPass();
        //createGraphicsPipeline();
        _swapchain->RegenerateFramebuffers();
        createUniformBuffers();
        //createDescriptorPool();
       // createDescriptorSets();
        /*U64 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            updateDescriptorSet( i, (VulkanImage*)_textures[_currentTextureIndex]->GetImage(), _textureSamplers[_currentTextureIndex] );
        }*/
        createCommandBuffers();

        _recreatingSwapchain = false;
    }

    void VulkanRenderer::createBuffers() {

        // Vertex buffer.
        _vertexBuffer = new VulkanVertex3DBuffer( _device );
        // Allocate 128 MiB for the vertex buffer
        _vertexBuffer->Allocate( 128 * 1024 * 1024 );

        // Index buffer.
        _indexBuffer = new VulkanIndexBuffer( _device );
        // Allocate 32 MiB for the index buffer.
        _indexBuffer->Allocate( 32 * 1024 * 1024 );
    }
}