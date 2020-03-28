
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

#include "../../StandardUniformBufferObject.h"
#include "../../../Resources/ITexture.h"

// TODO: temp
#include "../../../Assets/StaticMesh/StaticMesh.h"
#include "../../../Assets/StaticMesh/Loaders/OBJLoader.h"

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
        createShader( "main" );


        Extent2D extent = _platform->GetFramebufferExtent();
        _swapchain = new VulkanSwapchain( _device, _surface, _platform->GetWindow(), extent.Width, extent.Height );

        createRenderPass();
        _swapchain->RegenerateFramebuffers();

        createDescriptorSetLayout();

        createGraphicsPipeline();


        // Asset loading here for now

        // Load texture image/views
        _textures[0] = (VulkanTexture*)GetTexture( "assets/textures/test512.png" );
        _textures[1] = (VulkanTexture*)GetTexture( "assets/textures/testice.jpg" );

        // Create samplers
        createTextureSampler( &_textureSamplers[0] );
        createTextureSampler( &_textureSamplers[1] );

        // TODO: load model
        createBuffers();

        // End asset loading.

        // Create UBOs
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        U64 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            updateDescriptorSet( i, (VulkanImage*)_textures[_currentTextureIndex]->GetImage(), _textureSamplers[_currentTextureIndex] );
        }

        createCommandBuffers();
        createSyncObjects();

        // Listen for resize events.
        Event::Listen( EventType::WINDOW_RESIZED, this );

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

        vkFreeCommandBuffers( _device->LogicalDevice, _device->CommandPool, _commandBuffers.size(), _commandBuffers.data() );
        //vkFreeDescriptorSets( _device->LogicalDevice, _descriptorPool, swapchainImageCount, _descriptorSets.data() );
        vkDestroyDescriptorPool( _device->LogicalDevice, _descriptorPool, nullptr );

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
        vkDestroySampler( _device->LogicalDevice, _textureSamplers[0], nullptr );
        vkDestroySampler( _device->LogicalDevice, _textureSamplers[1], nullptr );
        delete _textures[0];
        _textures[0] = nullptr;
        delete _textures[1];
        _textures[1] = nullptr;



        vkDestroyPipeline( _device->LogicalDevice, _pipeline, nullptr );
        vkDestroyPipelineLayout( _device->LogicalDevice, _pipelineLayout, nullptr );

        vkDestroyDescriptorSetLayout( _device->LogicalDevice, _descriptorSetLayout, nullptr );
        VulkanRenderPassManager::DestroyRenderPass( _device, "RenderPass.Default" );

        if( _swapchain ) {
            delete _swapchain;
            _swapchain = nullptr;
        }

        vkDestroyShaderModule( _device->LogicalDevice, _vertexShaderModule, nullptr );
        vkDestroyShaderModule( _device->LogicalDevice, _fragmentShaderModule, nullptr );

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

    const bool VulkanRenderer::Frame( const F32 deltaTime ) {

        // If currently recreating the swapchain, boot out.
        if( _recreatingSwapchain ) {
            return true;
        }

        if( _framebufferResizeOccurred ) {
            _framebufferResizeOccurred = false;
            recreateSwapchain();
            return true;
        }

        // The operations here are asynchronous and not guaranteed to happen in order. Manage
        // this with semaphores and fences.

        // Wait for the execution of the current frame to complete. The fence being free will allow this to move on.
        if( !_inFlightFences[_swapchain->GetCurrentFrameIndex()]->Wait( U64_MAX ) ) {
            Logger::Warn( "In-flight fence wait failure!" );
        }

        // Acquire next image from the swap chain.
        U32 imageIndex;
        if( !_swapchain->AcquireNextImageIndex( U64_MAX, _imageAvailableSemaphores[_swapchain->GetCurrentFrameIndex()], nullptr, &imageIndex ) ) {
            return true;
        }

        // Update descriptors if need be. TEST: Swap texture
        // Must happen before queue is started below.
        _updatesTemp++;
        if( _updatesTemp > 3000 ) {
            _currentTextureIndex = ( _currentTextureIndex == 0 ? 1 : 0 );
            _updatesTemp = 0;
        }
        updateDescriptorSet( imageIndex, (VulkanImage*)_textures[_currentTextureIndex]->GetImage(), _textureSamplers[_currentTextureIndex] );

        // /////////////////////// BEGIN COMMAND BUFFERS ////////////////////////
        // Prepare commands for the queue.
        // Begin recording.
        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;
        VK_CHECK( vkBeginCommandBuffer( _commandBuffers[imageIndex], &beginInfo ) );

        // Begin the render pass. TODO: Should probably create these once and reuse.
        VulkanRenderPass* renderPass = VulkanRenderPassManager::GetRenderPass( "RenderPass.Default" );
        RenderPassClearInfo clearInfo;
        clearInfo.Color.Set( 0.0f, 0.0f, 0.2f, 0.0f );
        clearInfo.RenderArea.Set( 0, 0, _swapchain->Extent.width, _swapchain->Extent.height );
        clearInfo.Depth = 1.0f;
        clearInfo.Stencil = 0;
        renderPass->Begin( clearInfo, _swapchain->GetFramebuffer( imageIndex ), _commandBuffers[imageIndex] );

        // Bind the buffer to the graphics pipeline
        vkCmdBindPipeline( _commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline );

        // TODO: These should be provided by the front end instead.
        const LinkedListNode<VulkanBufferDataBlock>* vBlock = _vertexBuffer->PeekDataBlock();
        const LinkedListNode<VulkanBufferDataBlock>* iBlock = _indexBuffer->PeekDataBlock();

        //for( U64 i = 0; i < vertexBufferRanges.size(); ++i ) {
        while( vBlock != nullptr ) {
            // Bind vertex buffers
            VkBuffer vertexBuffers[] = { _vertexBuffer->GetHandle() };
            VkDeviceSize offsets[] = { vBlock->Value.Offset }; // was 0
            vkCmdBindVertexBuffers( _commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets );

            // Bind index buffer
            vkCmdBindIndexBuffer( _commandBuffers[imageIndex], _indexBuffer->GetHandle(), iBlock->Value.Offset, VK_INDEX_TYPE_UINT32 ); // offset was 0

            // Bind descriptor sets (UBOs and samplers)
            vkCmdBindDescriptorSets( _commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSets[imageIndex], 0, nullptr );

            // Make the draw call. TODO: use object properties
            //vkCmdDraw( _commandBuffers[imageIndex], 3, 1, 0, 0 );

            vkCmdDrawIndexed( _commandBuffers[imageIndex], (U32)iBlock->Value.ElementCount, 1, 0, 0, 0 ); // _indexBuffer->GetElementCount()

            vBlock = vBlock->Next;
            iBlock = iBlock->Next;
        }

        // End render pass.
        renderPass->End( _commandBuffers[imageIndex] );

        // End recording
        VK_CHECK( vkEndCommandBuffer( _commandBuffers[imageIndex] ) );

        // ////////////////// END COMMAND BUFFERS ///////////////////////////////

        // Check if a previous frame is using this image (i.e. its fence is being waited on)
        U8 frameidx = _swapchain->GetCurrentFrameIndex();
        if( _inFlightImageFences[frameidx] != VK_NULL_HANDLE ) {
            _inFlightImageFences[frameidx]->Wait( U64_MAX );
        }

        // Mark the image fence as in-use by this frame.
        _inFlightImageFences[frameidx] = _inFlightFences[frameidx];

        updateUniformBuffers( imageIndex );



        // Submit the queue and wait for the operation to complete.
        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

        VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[frameidx]->GetHandle() };
        VkSemaphore signalSemaphores[] = { _renderCompleteSemaphores[frameidx]->GetHandle() };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores; // Must be signaled from an operation completion before these commands will run.
        submitInfo.pWaitDstStageMask = waitStages; // Array aligning with pWaitSemaphores indicating which stage the wait will occur.
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &_commandBuffers[imageIndex]; // Command buffers to be executed (primary only).

        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // Reset the fence for use on the next frame.
        _inFlightImageFences[frameidx]->Reset();

        // Submit the queue
        VK_CHECK( vkQueueSubmit( _device->GraphicsQueue, 1, &submitInfo, _inFlightFences[frameidx]->GetHandle() ) );

        // Give the image back to the swapchain.
        _swapchain->Present( _device->GraphicsQueue, _device->PresentationQueue, _renderCompleteSemaphores[frameidx], imageIndex );

        return true;
    }

    void VulkanRenderer::OnEvent( const Event& event ) {
        switch( event.Type ) {
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

    ITexture* VulkanRenderer::GetTexture( const char* path ) {
        // TODO: Should probably get by name somehow.
        return new VulkanTexture( _device, path, path );
    }

    Extent2D VulkanRenderer::GetFramebufferExtent() {
        return _platform->GetFramebufferExtent();
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

    void VulkanRenderer::createShader( const char* name ) {

        // Vertex shader
        U64 vertShaderSize;
        char* vertexShaderSource = readShaderFile( name, "vert", &vertShaderSize );
        VkShaderModuleCreateInfo vertexShaderCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        vertexShaderCreateInfo.codeSize = vertShaderSize;
        vertexShaderCreateInfo.pCode = (U32*)vertexShaderSource;
        VK_CHECK( vkCreateShaderModule( _device->LogicalDevice, &vertexShaderCreateInfo, nullptr, &_vertexShaderModule ) );

        // Fragment shader
        U64 fragShaderSize;
        char* fragShaderSource = readShaderFile( name, "frag", &fragShaderSize );
        VkShaderModuleCreateInfo fragShaderCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        fragShaderCreateInfo.codeSize = fragShaderSize;
        fragShaderCreateInfo.pCode = (U32*)fragShaderSource;
        VK_CHECK( vkCreateShaderModule( _device->LogicalDevice, &fragShaderCreateInfo, nullptr, &_fragmentShaderModule ) );

        // Vertex shader stage
        VkPipelineShaderStageCreateInfo vertShaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = _vertexShaderModule;
        vertShaderStageInfo.pName = "main";

        // Fragment shader stage
        VkPipelineShaderStageCreateInfo fragShaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = _fragmentShaderModule;
        fragShaderStageInfo.pName = "main";

        _shaderStageCount = 2;
        _shaderStages.push_back( vertShaderStageInfo );
        _shaderStages.push_back( fragShaderStageInfo );

        delete vertexShaderSource;
        delete fragShaderSource;
    }

    char* VulkanRenderer::readShaderFile( const char* filename, const char* shaderType, U64* fileSize ) {
        char buffer[256];
        I32 length = snprintf( buffer, 256, "shaders/%s.%s.spv", filename, shaderType );
        if( length < 0 ) {
            Logger::Fatal( "Shader filename is too long." );
        }
        buffer[length] = '\0';

        std::ifstream file( buffer, std::ios::ate | std::ios::binary );
        if( !file.is_open() ) {
            Logger::Fatal( "Shader unable to open file." );
        }

        *fileSize = (U64)file.tellg();
        char* fileBuffer = (char*)malloc( *fileSize );
        file.seekg( 0 );
        file.read( fileBuffer, *fileSize );
        file.close();

        return fileBuffer;
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

    void VulkanRenderer::createGraphicsPipeline() {

        VkExtent2D extent = _swapchain->Extent;

        // Viewport
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = (F32)extent.height;
        viewport.width = (F32)extent.width;
        viewport.height = -(F32)extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Scissor
        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = { extent.width, extent.height };

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
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &_descriptorSetLayout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        VK_CHECK( vkCreatePipelineLayout( _device->LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout ) );

        // Pipeline create
        VkGraphicsPipelineCreateInfo pipelineCreateInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipelineCreateInfo.stageCount = (U32)_shaderStageCount;
        pipelineCreateInfo.pStages = _shaderStages.data();
        pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssembly;;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
        pipelineCreateInfo.pDepthStencilState = &depthStencil;
        pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        pipelineCreateInfo.pDynamicState = nullptr;

        pipelineCreateInfo.layout = _pipelineLayout;

        // TODO: Get once and save
        pipelineCreateInfo.renderPass = VulkanRenderPassManager::GetRenderPass( "RenderPass.Default" )->GetHandle();
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        VK_CHECK( vkCreateGraphicsPipelines( _device->LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_pipeline ) );

        Logger::Log( "Graphics pipeline created!" );
    }



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

    void VulkanRenderer::updateDescriptorSet( U64 descriptorSetIndex, VulkanImage* textureImage, VkSampler sampler ) {

        // Configure the descriptors for the given index..
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = _uniformBuffers[descriptorSetIndex]->GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof( StandardUniformBufferObject );

        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureImage->GetView();
        imageInfo.sampler = sampler;

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
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets( _device->LogicalDevice, 2, descriptorWrites.data(), 0, nullptr );
    }



    void VulkanRenderer::createCommandBuffers() {
        U32 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocateInfo.commandPool = _device->CommandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = swapchainImageCount;

        _commandBuffers.resize( swapchainImageCount );

        VK_CHECK( vkAllocateCommandBuffers( _device->LogicalDevice, &commandBufferAllocateInfo, _commandBuffers.data() ) );
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
        vkFreeCommandBuffers( _device->LogicalDevice, _device->CommandPool, (U32)_commandBuffers.size(), _commandBuffers.data() );
        _commandBuffers.clear();

        vkDestroyPipeline( _device->LogicalDevice, _pipeline, nullptr );
        vkDestroyPipelineLayout( _device->LogicalDevice, _pipelineLayout, nullptr );

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
        createGraphicsPipeline();
        _swapchain->RegenerateFramebuffers();
        //createDepthResources();
        //createFramebuffers();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        U64 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            updateDescriptorSet( i, (VulkanImage*)_textures[_currentTextureIndex]->GetImage(), _textureSamplers[_currentTextureIndex] );
        }
        createCommandBuffers();

        _recreatingSwapchain = false;
    }

    void VulkanRenderer::createBuffers() {
        // Populate with data
        //std::vector<Vertex3D> verts( 8 );
        //verts[0] = { {-0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} };
        //verts[1] = { { 0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} };
        //verts[2] = { { 0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} };
        //verts[3] = { {-0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} };
        //verts[4] = { {-0.5f, -0.5f,  -1.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} };
        //verts[5] = { { 0.5f, -0.5f,  -1.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} };
        //verts[6] = { { 0.5f,  0.5f,  1.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} };
        //verts[7] = { {-0.5f,  0.5f,  -1.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} };

        //std::vector<U32> indices = {
        //    0, 1, 2, 2, 3, 0,
        //    4, 5, 6, 6, 7, 4
        //};

        //// Vertex buffer.
        //_vertexBuffer = new VulkanVertex3DBuffer( this );
        //_vertexBuffer->SetData( verts );

        //// Index buffer.
        //_indexBuffer = new VulkanIndexBuffer( this );
        //_indexBuffer->SetData( indices );

        // Load from obj.
        std::vector<StaticMesh> meshes = OBJLoader::LoadObjFile( "assets/models/sibenik.obj" );

        // For now, just get the total size needed for the meshes.
        U64 totalVertSize = 0;
        U64 totalIndexSize = 0;
        for( auto mesh : meshes ) {
            totalVertSize += ( sizeof( Vertex3D ) * mesh.Vertices.size() );
            totalIndexSize += ( sizeof( U32 ) * mesh.Indices.size() );
        }
        Logger::Log( "Mesh verts require %.2f MiB", ( (F32)totalVertSize / 1024.0f / 1024.0f ) );
        Logger::Log( "Mesh indices require %.2f MiB", ( (F32)totalIndexSize / 1024.0f / 1024.0f ) );

        // Vertex buffer.
        _vertexBuffer = new VulkanVertex3DBuffer( _device );
        // Allocate 128 MiB for the vertex buffer
        _vertexBuffer->Allocate( 128 * 1024 * 1024 );

        // Index buffer.
        _indexBuffer = new VulkanIndexBuffer( _device );
        // Allocate 32 MiB for the index buffer.
        _indexBuffer->Allocate( 32 * 1024 * 1024 );

        U64 vertOffset = 0;
        U64 indexOffset = 0;
        for( auto mesh : meshes ) {
            if( mesh.Vertices.size() == 0 || mesh.Indices.size() == 0 ) {
                continue;
            }
            _vertexBuffer->SetDataRange( mesh.Vertices, vertOffset );
            _indexBuffer->SetDataRange( mesh.Indices, indexOffset );
            vertOffset += ( sizeof( Vertex3D ) * mesh.Vertices.size() );
            indexOffset += ( sizeof( U32 ) * mesh.Indices.size() );
        }
    }

    void VulkanRenderer::createTextureSampler( VkSampler* sampler ) {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = 16;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        VK_CHECK( vkCreateSampler( _device->LogicalDevice, &samplerInfo, nullptr, &*sampler ) );
    }
}