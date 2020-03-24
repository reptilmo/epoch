
#include <vector>
#include <fstream>

// TODO: temp
#include <chrono>

#include "../../../Platform.h"
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

#include "VulkanRenderer.h"

namespace Epoch {

    VulkanRenderer::VulkanRenderer( Platform* platform ) {
        _platform = platform;
        Logger::Trace( "Initializing Vulkan renderer..." );

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
        std::vector<const char*> requiredValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        // Get available layers.
        U32 availableLayerCount = 0;
        VK_CHECK( vkEnumerateInstanceLayerProperties( &availableLayerCount, nullptr ) );
        std::vector<VkLayerProperties> availableLayers( availableLayerCount );
        VK_CHECK( vkEnumerateInstanceLayerProperties( &availableLayerCount, availableLayers.data() ) );

        // Verify that all required layers are available.
        bool success = true;
        for( U32 i = 0; i < (U32)requiredValidationLayers.size(); ++i ) {
            bool found = false;
            for( U32 j = 0; j < availableLayerCount; ++j ) {
                if( strcmp( requiredValidationLayers[i], availableLayers[j].layerName ) == 0 ) {
                    found = true;
                    break;
                }
            }

            if( !found ) {
                success = false;
                Logger::Fatal( "Required validation layer is missing: %s", requiredValidationLayers[i] );
                break;
            }
        }

        instanceCreateInfo.enabledLayerCount = (U32)requiredValidationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();

        // Create instance
        VK_CHECK( vkCreateInstance( &instanceCreateInfo, nullptr, &_instance ) );

        // Create debugger
        _debugger = new VulkanDebugger( _instance, VulkanDebuggerLevel::ERROR | VulkanDebuggerLevel::WARNING );

        // Create the surface
        _platform->CreateSurface( _instance, &_surface );

        // Create VulkanDevice
        _device = new VulkanDevice( _instance, requiredValidationLayers, _surface );

        // Shader creation.
        createShader( "main" );

        createSwapchain();
        createSwapchainImagesAndViews();

        createRenderPass();

        createDescriptorSetLayout();

        createGraphicsPipeline();

        //createCommandPool();

        createDepthResources();
        createFramebuffers();

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
        U64 swapchainImageCount = _swapchainImages.size();
        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            updateDescriptorSet( i, _textures[_currentTextureIndex]->GetImage(), _textureSamplers[_currentTextureIndex] );
        }

        createCommandBuffers();
        createSyncObjects();

        // Listen for resize events.
        Event::Listen( EventType::WINDOW_RESIZED, this );
    }

    VulkanRenderer::~VulkanRenderer() {

        VK_CHECK( vkDeviceWaitIdle( _device->LogicalDevice ) );

        // sync objects
        for( U8 i = 0; i < _maxFramesInFlight; ++i ) {
            vkDestroySemaphore( _device->LogicalDevice, _imageAvailableSemaphores[i], nullptr );
            vkDestroySemaphore( _device->LogicalDevice, _renderCompleteSemaphores[i], nullptr );
            vkDestroyFence( _device->LogicalDevice, _inFlightFences[i], nullptr );
        }
        U64 swapchainImageCount = _swapchainImages.size();

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

        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            vkDestroyFramebuffer( _device->LogicalDevice, _swapchainFramebuffers[i], nullptr );
        }

        if( _depthImage ) {
            delete _depthImage;
            _depthImage = nullptr;
        }

        vkDestroyPipeline( _device->LogicalDevice, _pipeline, nullptr );
        vkDestroyPipelineLayout( _device->LogicalDevice, _pipelineLayout, nullptr );

        vkDestroyDescriptorSetLayout( _device->LogicalDevice, _descriptorSetLayout, nullptr );
        vkDestroyRenderPass( _device->LogicalDevice, _renderPass, nullptr );

        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            vkDestroyImageView( _device->LogicalDevice, _swapchainImageViews[i], nullptr );
            //vkDestroyImage( _device->LogicalDevice, _swapchainImages[i], nullptr );
        }

        vkDestroySwapchainKHR( _device->LogicalDevice, _swapchain, nullptr );

        vkDestroyShaderModule( _device->LogicalDevice, _vertexShaderModule, nullptr );
        vkDestroyShaderModule( _device->LogicalDevice, _fragmentShaderModule, nullptr );

        vkDestroySurfaceKHR( _instance, _surface, nullptr );

        if( _debugger ) {
            delete _debugger;
            _debugger = nullptr;
        }

        //cleanupSwapchain();

        if( _device ) {
            delete _device;
            _device = nullptr;
        }

        if( _instance ) {
            vkDestroyInstance( _instance, nullptr );
            _instance = nullptr;
        }
    }

    const bool VulkanRenderer::Initialize() {


        return true;
    }

    const bool VulkanRenderer::Frame( const F32 deltaTime ) {

        // If currently recreating the swapchain, boot out.
        if( _recreatingSwapchain ) {
            return true;
        }

        // The operations here are asynchronous and not guaranteed to happen in order. Manage
        // this with semaphores and fences.

        // Wait for the execution of the current frame to complete. The fence being free will allow this to move on.
        vkWaitForFences( _device->LogicalDevice, 1, &_inFlightFences[_currentFrameIndex], VK_TRUE, U64_MAX );

        // Acquire next image from the swap chain.
        U32 imageIndex;
        VkResult result = vkAcquireNextImageKHR( _device->LogicalDevice, _swapchain, U64_MAX, _imageAvailableSemaphores[_currentFrameIndex], VK_NULL_HANDLE, &imageIndex );
        if( result == VkResult::VK_ERROR_OUT_OF_DATE_KHR ) {
            // Trigger swapchain recreation, then boot out of the render loop.
            recreateSwapchain();
            return true;
        } else if( result != VkResult::VK_SUCCESS && result != VkResult::VK_SUBOPTIMAL_KHR ) {
            Logger::Error( "Failed to acquire swapchain image!" );
            return false;
        }

        // Update descriptors if need be. TEST: Swap texture
        // Must happen before queue is started below.
        _updatesTemp++;
        if( _updatesTemp > 3000 ) {
            _currentTextureIndex = ( _currentTextureIndex == 0 ? 1 : 0 );
            _updatesTemp = 0;
        }
        updateDescriptorSet( imageIndex, _textures[_currentTextureIndex]->GetImage(), _textureSamplers[_currentTextureIndex] );

        // /////////////////////// BEGIN COMMAND BUFFERS ////////////////////////
        // Prepare commands for the queue.
        // Begin recording.
        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;
        VK_CHECK( vkBeginCommandBuffer( _commandBuffers[imageIndex], &beginInfo ) );

        // Begin the render pass.
        VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.renderPass = _renderPass;
        renderPassBeginInfo.framebuffer = _swapchainFramebuffers[imageIndex];
        renderPassBeginInfo.renderArea.offset = { 0,0 };
        renderPassBeginInfo.renderArea.extent = _swapchainExtent;

        // TODO: Clear colour and depth based on configuration.
        VkClearValue clearValues[2] = {};
        clearValues[0].color = { 0.0f, 0.0f, 0.2f, 0.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;

        // TODO: make configurable
        vkCmdBeginRenderPass( _commandBuffers[imageIndex], &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE );

        // Bind the buffer to the graphics pipeline
        vkCmdBindPipeline( _commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline );

        // TODO: These should be provided by the front end instead.
        //auto vertexBufferRanges = _vertexBuffer->GetDataRanges();
        //auto indexBufferRanges = _indexBuffer->GetDataRanges();
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
        vkCmdEndRenderPass( _commandBuffers[imageIndex] );

        // End recording
        VK_CHECK( vkEndCommandBuffer( _commandBuffers[imageIndex] ) );

        // ////////////////// END COMMAND BUFFERS ///////////////////////////////

        // Check if a previous frame is using this image (i.e. its fence is being waited on)
        if( _inFlightImageFences[_currentFrameIndex] != VK_NULL_HANDLE ) {
            vkWaitForFences( _device->LogicalDevice, 1, &_inFlightImageFences[_currentFrameIndex], VK_TRUE, U64_MAX );
        }

        // Mark the image fence as in-use by this frame.
        _inFlightImageFences[_currentFrameIndex] = _inFlightFences[_currentFrameIndex];

        updateUniformBuffers( imageIndex );



        // Submit the queue and wait for the operation to complete.
        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

        VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrameIndex] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores; // Must be signaled from an operation completion before these commands will run.
        submitInfo.pWaitDstStageMask = waitStages; // Array aligning with pWaitSemaphores indicating which stage the wait will occur.
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &_commandBuffers[imageIndex]; // Command buffers to be executed (primary only).

        VkSemaphore signalSemaphores[] = { _renderCompleteSemaphores[_currentFrameIndex] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // Reset the fence for use on the next frame.
        vkResetFences( _device->LogicalDevice, 1, &_inFlightImageFences[_currentFrameIndex] );

        // Submit the queue
        VK_CHECK( vkQueueSubmit( _device->GraphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrameIndex] ) );

        // Return the image to the swapchain for presentation
        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &_swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        result = vkQueuePresentKHR( _device->PresentationQueue, &presentInfo );
        if( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResizeOccurred ) {
            // Swapchain is out of date, suboptimal or a framebuffer resize hjas occurred. Trigger swapchain recreation.
            _framebufferResizeOccurred = false;
            recreateSwapchain();
        } else if( result != VkResult::VK_SUCCESS ) {
            Logger::Error( "Failed to present swap chain image!" );
            return false;
        }

        // Increment (and loop) the index.
        _currentFrameIndex = ( _currentFrameIndex + 1 ) % _maxFramesInFlight;

        return true;
    }

    void VulkanRenderer::OnEvent( const Event& event ) {
        switch( event.Type ) {
        case EventType::WINDOW_RESIZED:
            //const WindowResizedEvent wre = (const WindowResizedEvent)event;
            _framebufferResizeOccurred = true;
            break;
        default:
            break;
        }
    }



    ITexture* VulkanRenderer::GetTexture( const char* path ) {
        // TODO: Should probably get by name somehow.
        return new VulkanTexture( _device, path, path );
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

    void VulkanRenderer::createSwapchain() {
        VulkanSwapchainSupportDetails swapchainSupport = _device->SwapchainSupport;
        VkSurfaceCapabilitiesKHR capabilities = swapchainSupport.Capabilities;

        // Choose a swap surface format
        bool found = false;
        for( auto format : swapchainSupport.Formats ) {

            // Preferred formats 
            if( format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
                _swapchainImageFormat = format;
                found = true;
                break;
            }
        }

        if( !found ) {
            _swapchainImageFormat = swapchainSupport.Formats[0];
        }

        // Choose a present mode
        VkPresentModeKHR presentMode;
        found = false;
        for( auto mode : swapchainSupport.PresentationModes ) {

            // If preferred mode is available, use it
            if( mode == VK_PRESENT_MODE_MAILBOX_KHR ) {
                presentMode = mode;
                found = true;
            }
        }

        if( !found ) {
            presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        // Swapchain extent
        if( capabilities.currentExtent.width != U32_MAX ) {
            _swapchainExtent = capabilities.currentExtent;
        } else {
            Extent2D extent = _platform->GetFramebufferExtent();
            _swapchainExtent = { (U32)extent.Width, (U32)extent.Height };

            // Clamp to a value allowed by the GPU.
            _swapchainExtent.width = TMath::ClampU32( _swapchainExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
            _swapchainExtent.height = TMath::ClampU32( _swapchainExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
        }

        U32 imageCount = capabilities.minImageCount + 1;

        if( capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount ) {
            imageCount = capabilities.maxImageCount;
        }

        // Swapchain create info
        VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = _surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = _swapchainImageFormat.format;
        swapchainCreateInfo.imageColorSpace = _swapchainImageFormat.colorSpace;
        swapchainCreateInfo.imageExtent = _swapchainExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Setup the queue family indices
        if( _device->GraphicsFamilyQueueIndex != _device->PresentationFamilyQueueIndex ) {
            U32 queueFamilyIndices[] = { (U32)_device->GraphicsFamilyQueueIndex, (U32)_device->PresentationFamilyQueueIndex };
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        swapchainCreateInfo.preTransform = capabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = nullptr;

        VK_CHECK( vkCreateSwapchainKHR( _device->LogicalDevice, &swapchainCreateInfo, nullptr, &_swapchain ) );
    }

    void VulkanRenderer::createSwapchainImagesAndViews() {

        // Images
        U32 swapchainImageCount = 0;
        vkGetSwapchainImagesKHR( _device->LogicalDevice, _swapchain, &swapchainImageCount, nullptr );
        _swapchainImages.resize( swapchainImageCount );
        _swapchainImageViews.resize( swapchainImageCount );
        vkGetSwapchainImagesKHR( _device->LogicalDevice, _swapchain, &swapchainImageCount, _swapchainImages.data() );

        for( U32 i = 0; i < swapchainImageCount; ++i ) {
            VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            viewInfo.image = _swapchainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = _swapchainImageFormat.format;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            VK_CHECK( vkCreateImageView( _device->LogicalDevice, &viewInfo, nullptr, &_swapchainImageViews[i] ) );
        }
    }

    void VulkanRenderer::createRenderPass() {

        // Color attachment
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = _swapchainImageFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // per-pixel samples.
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear contents.
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Preserve after rendering
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Do not expect any particular layout before render pass starts.
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Transitioned to after the render pass

        // Color attachment reference
        VkAttachmentReference colorAttachmentReference = {};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


        // Depth attachment
        VkAttachmentDescription depthAttachment = {};
        depthAttachment.format = _device->DepthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Depth attachment reference
        VkAttachmentReference depthAttachmentReference = {};
        depthAttachmentReference.attachment = 1;
        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Subpass
        VkSubpassDescription subpass = {};

        // Graphics or compute - in this case, graphics.
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        // Input from a shader
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = nullptr;

        // Color buffer.
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;

        // Depth stencil data.
        subpass.pDepthStencilAttachment = &depthAttachmentReference;

        // Attachments used for multisampling colour attachments.
        subpass.pResolveAttachments = nullptr;

        // Attachments not used in this subpass, but must be preserved.
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = nullptr;

        // Render pass dependencies
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        const U32 attachmentCount = 2;
        VkAttachmentDescription attachments[attachmentCount] = {
            colorAttachment,
            depthAttachment
        };

        // Render pass create.
        VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        renderPassCreateInfo.attachmentCount = attachmentCount;
        renderPassCreateInfo.pAttachments = attachments;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &dependency;
        VK_CHECK( vkCreateRenderPass( _device->LogicalDevice, &renderPassCreateInfo, nullptr, &_renderPass ) );
    }

    void VulkanRenderer::createGraphicsPipeline() {

        // Viewport
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = (F32)_swapchainExtent.height;
        viewport.width = (F32)_swapchainExtent.width;
        viewport.height = -(F32)_swapchainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Scissor
        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = { _swapchainExtent.width, _swapchainExtent.height };

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
        pipelineCreateInfo.renderPass = _renderPass;
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        VK_CHECK( vkCreateGraphicsPipelines( _device->LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_pipeline ) );

        Logger::Log( "Graphics pipeline created!" );
    }



    void VulkanRenderer::createDepthResources() {
        _depthFormat = VulkanUtilities::FindDepthFormat( _device->PhysicalDevice );
        VulkanImageCreateInfo depthImageCreateInfo;
        depthImageCreateInfo.Width = _swapchainExtent.width;
        depthImageCreateInfo.Height = _swapchainExtent.height;
        depthImageCreateInfo.Type = VK_IMAGE_TYPE_2D;
        depthImageCreateInfo.Format = _depthFormat;
        depthImageCreateInfo.Tiling = VK_IMAGE_TILING_OPTIMAL;
        depthImageCreateInfo.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImageCreateInfo.Properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        depthImageCreateInfo.CreateView = true;
        depthImageCreateInfo.ViewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;

        VulkanImage::Create( _device, depthImageCreateInfo, &_depthImage );
    }

    void VulkanRenderer::createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof( StandardUniformBufferObject );

        U64 swapchainImageCount = _swapchainImages.size();
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

        StandardUniformBufferObject ubo = {};
        Rotator rotator;
        rotator.Pitch = time * 90.0f * 0.1f;
        rotator.Roll = 90.0f;
        ubo.Model = Matrix4x4::Identity();
        ubo.Model *= Matrix4x4::Translation( Vector3( 0.0f, 0.0f, 0.0f ) );
        ubo.Model *= rotator.ToQuaternion().ToMatrix4x4();
        ubo.View = Matrix4x4::LookAt( Vector3( 0.0f, 25.0f, 25.0f ), Vector3::Zero(), Vector3::Up() );
        ubo.Projection = Matrix4x4::Perspective( TMath::DegToRad( 90.0f ), (F32)_swapchainExtent.width / (F32)_swapchainExtent.height, 0.1f, 1000.0f );

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
        VkDescriptorPoolSize poolSizes[2];
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = (U32)_swapchainImages.size();
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = (U32)_swapchainImages.size();;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 2;
        poolInfo.pPoolSizes = poolSizes;
        poolInfo.maxSets = (U32)_swapchainImages.size();

        VK_CHECK( vkCreateDescriptorPool( _device->LogicalDevice, &poolInfo, nullptr, &_descriptorPool ) );
    }
    void VulkanRenderer::createDescriptorSets() {
        U64 swapchainImageCount = _swapchainImages.size();
        // Fill an array with pointers to the descriptor set layout.
        std::vector<VkDescriptorSetLayout> layouts( swapchainImageCount, _descriptorSetLayout );

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.descriptorSetCount = (U32)swapchainImageCount;
        allocInfo.pSetLayouts = layouts.data();

        _descriptorSets.resize( _swapchainImages.size() );
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

    void VulkanRenderer::createFramebuffers() {
        U32 swapchainImageCount = (U32)_swapchainImageViews.size();
        _swapchainFramebuffers.resize( swapchainImageCount );

        for( U32 i = 0; i < swapchainImageCount; ++i ) {
            U32 attachmentCount = 2; // TODO: Make this dynamic based on currently configured attachments.
            VkImageView attachments[] = {
                _swapchainImageViews[i],
                _depthImage->GetView()
            };

            VkFramebufferCreateInfo framebufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
            framebufferCreateInfo.renderPass = _renderPass;
            framebufferCreateInfo.attachmentCount = attachmentCount;
            framebufferCreateInfo.pAttachments = attachments;
            framebufferCreateInfo.width = _swapchainExtent.width;
            framebufferCreateInfo.height = _swapchainExtent.height;
            framebufferCreateInfo.layers = 1;

            VK_CHECK( vkCreateFramebuffer( _device->LogicalDevice, &framebufferCreateInfo, nullptr, &_swapchainFramebuffers[i] ) );
        }
    }

    void VulkanRenderer::createCommandBuffers() {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocateInfo.commandPool = _device->CommandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = (U32)_swapchainImageViews.size();

        _commandBuffers.resize( _swapchainImageViews.size() );

        VK_CHECK( vkAllocateCommandBuffers( _device->LogicalDevice, &commandBufferAllocateInfo, _commandBuffers.data() ) );
    }

    void VulkanRenderer::createSyncObjects() {
        _imageAvailableSemaphores.resize( _maxFramesInFlight );
        _renderCompleteSemaphores.resize( _maxFramesInFlight );
        _inFlightFences.resize( _maxFramesInFlight );

        for( U8 i = 0; i < _maxFramesInFlight; ++i ) {
            VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
            VK_CHECK( vkCreateSemaphore( _device->LogicalDevice, &semaphoreCreateInfo, nullptr, &_imageAvailableSemaphores[i] ) );
            VK_CHECK( vkCreateSemaphore( _device->LogicalDevice, &semaphoreCreateInfo, nullptr, &_renderCompleteSemaphores[i] ) );

            VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };

            // Create the fence in a signaled state, indicating that the first frame has already been "rendered".
            // This will prevent the application from waiting indefinitely for the first frame to render since it
            // cannot be rendered until a frame is "rendered" before it.
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            VK_CHECK( vkCreateFence( _device->LogicalDevice, &fenceCreateInfo, nullptr, &_inFlightFences[i] ) );
        }

        // In flight fences should not yet exist at this point, so clear the list.
        _inFlightImageFences.resize( _swapchainImages.size() );
        for( U64 i = 0; i < _swapchainImages.size(); ++i ) {
            _inFlightImageFences[i] = nullptr;
        }
    }

    void VulkanRenderer::cleanupSwapchain() {
        if( _depthImage ) {
            delete _depthImage;
        }

        for( U64 i = 0; i < _swapchainImages.size(); ++i ) {
            vkDestroyFramebuffer( _device->LogicalDevice, _swapchainFramebuffers[i], nullptr );
        }

        // Free the command buffers in the pool, but do not destroy the pool.
        vkFreeCommandBuffers( _device->LogicalDevice, _device->CommandPool, (U32)_commandBuffers.size(), _commandBuffers.data() );
        _commandBuffers.clear();

        vkDestroyPipeline( _device->LogicalDevice, _pipeline, nullptr );
        vkDestroyPipelineLayout( _device->LogicalDevice, _pipelineLayout, nullptr );

        vkDestroyRenderPass( _device->LogicalDevice, _renderPass, nullptr );

        for( auto scImageView : _swapchainImageViews ) {
            vkDestroyImageView( _device->LogicalDevice, scImageView, nullptr );
        }
        _swapchainImages.clear();

        vkDestroySwapchainKHR( _device->LogicalDevice, _swapchain, nullptr );

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

        VK_CHECK( vkDeviceWaitIdle( _device->LogicalDevice ) );

        // Cleanup the old swapchain.
        cleanupSwapchain();

        _device->RequerySupport();

        createSwapchain();
        createSwapchainImagesAndViews();
        createRenderPass();
        createGraphicsPipeline();
        createDepthResources();
        createFramebuffers();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        U64 swapchainImageCount = _swapchainImages.size();
        for( U64 i = 0; i < swapchainImageCount; ++i ) {
            updateDescriptorSet( i, _textures[_currentTextureIndex]->GetImage(), _textureSamplers[_currentTextureIndex] );
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