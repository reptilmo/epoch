#include "../../../Platform/VulkanPlatform.h"
#include "../../../Platform/IApplication.h"
#include "../../../Platform/IWindow.h"
#include "../../../Logger.h"
#include "../../../Defines.h"
#include "../../../Memory/Memory.h"
#include "../../../Math/TMath.h"
#include "../../../Math/Rotator.h"
#include "../../../Math/Matrix4x4.h"
#include "../../../Math/Vector3.h"
#include "../../../Math/Quaternion.h"
#include "../../../String/TString.h"

#include "../../../Resources/ITexture.h"
#include "../../../Resources/StaticMesh.h"
#include "../../Material.h"
#include "../../IShader.h"

#include "../../../Events/Event.h"

#include "VulkanUtilities.h"
#include "VulkanImage.h"
#include "VulkanTexture.h"
#include "VulkanInternalBuffer.h"
#include "VulkanVertex3DBuffer.h"
#include "VulkanIndexBuffer.h"

#include "VulkanDevice.h"
#include "VulkanRenderPassManager.h"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanQueue.h"
#include "VulkanShader.h"

#include "VulkanRendererBackend.h"

#if VULKAN_USE_VALIDATION
#include "VulkanDebugger.h"
#endif

namespace Epoch {

    VulkanRendererBackend::VulkanRendererBackend( IApplication* application ) {
        _application = application;
        Logger::Trace( "Creating Vulkan renderer backend..." );
    }

    VulkanRendererBackend::~VulkanRendererBackend() {
        if( _instance ) {

            // Make sure things are destroyed.
            Destroy();
        }
    }

    const bool VulkanRendererBackend::Initialize() {
        Logger::Trace( "Initializing Vulkan renderer backend..." );

        createInstance();

        // Create debugger
#if VULKAN_USE_VALIDATION
        _debugger = new VulkanDebugger( _instance, VulkanDebuggerLevel::ERROR | VulkanDebuggerLevel::WARNING );
#endif

        IWindow* applicationWindow = _application->GetApplicationWindow();

        // Create the surface
        VulkanPlatform::CreateSurface( applicationWindow->GetHandle(), _instance, &_surface );

        // Create VulkanDevice
        _device = new VulkanDevice( _instance, _requiredValidationLayers, _surface );

        // HACK: This is here until the platform layer is removed.
        _device->FramebufferSize = applicationWindow->GetFramebufferExtent();

        // Create swapchain
        Extent2D extent = applicationWindow->GetFramebufferExtent();
        _swapchain = new VulkanSwapchain( _device, _surface, applicationWindow->GetHandle(), extent.Width, extent.Height );

        // Listen for resize events.
        Event::Listen( EventType::WINDOW_RESIZED, this );

        // Create renderpass.
        createRenderPass();
        _swapchain->RegenerateFramebuffers();

        // Built-in shader creation.
        _unlitShader = new VulkanUnlitShader( _device, _swapchain->GetSwapchainImageCount(), "RenderPass.Default" );

        createBuffers();
        createCommandBuffers();
        createSyncObjects();

        return true;
    }

    void VulkanRendererBackend::Shutdown() {
        _isShutDown = true;
        _device->WaitIdle();
    }

    void VulkanRendererBackend::Destroy() {
        _device->WaitIdle();

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

        if( _indexBuffer ) {
            delete _indexBuffer;
            _indexBuffer = nullptr;
        }

        if( _vertexBuffer ) {
            delete _vertexBuffer;
            _vertexBuffer = nullptr;
        }


        VulkanRenderPassManager::DestroyRenderPass( _device, "RenderPass.Default" );

        if( _unlitShader ) {
            delete _unlitShader;
            _unlitShader = nullptr;
        }

        if( _swapchain ) {
            delete _swapchain;
            _swapchain = nullptr;
        }

        vkDestroySurfaceKHR( _instance, _surface, nullptr );

#if VULKAN_USE_VALIDATION
        if( _debugger ) {
            delete _debugger;
            _debugger = nullptr;
        }
#endif

        if( _device ) {
            delete _device;
            _device = nullptr;
        }

        if( _instance ) {
            vkDestroyInstance( _instance, nullptr );
            _instance = nullptr;
        }
    }

    const bool VulkanRendererBackend::PrepareFrame( const F32 deltaTime ) {

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

        // Begin recording.
        VulkanCommandBuffer* currentCommandBuffer = _commandBuffers[_currentImageIndex];
        currentCommandBuffer->Reset();
        currentCommandBuffer->Begin();

        // Begin the render pass. TODO: Should probably create these once and reuse.
        VulkanRenderPass* renderPass = VulkanRenderPassManager::GetRenderPass( "RenderPass.Default" );
        RenderPassClearInfo clearInfo;
        clearInfo.Color.Set( 0.0f, 0.0f, 0.2f, 0.0f );
        clearInfo.RenderArea.Set( 0, 0, (F32)_swapchain->Extent.width, (F32)_swapchain->Extent.height );
        clearInfo.Depth = 1.0f;
        clearInfo.Stencil = 0;
        currentCommandBuffer->BeginRenderPass( clearInfo, _swapchain->GetFramebuffer( _currentImageIndex ), renderPass );

        // TEMP =======================================================================
        // TODO: obtain from camera
        Matrix4x4 view = Matrix4x4::LookAt( Vector3( 0.0f, 25.0f, 25.0f ), Vector3::Zero(), Vector3::Up() );
        //Matrix4x4 view = Matrix4x4::LookAt( Vector3( 0.0f, 0.0f, 25.0f ), Vector3::Zero(), Vector3::Up() );

        // TODO: obtain from private var, set only on change of camera/window
        VkExtent2D extent = _swapchain->Extent;
        Matrix4x4 projection = Matrix4x4::Perspective( TMath::DegToRad( 90.0f ), (F32)extent.width / (F32)extent.height, 0.1f, 1000.0f );
        Matrix4x4 correction;
        correction.Data()[0] = 1.0f;
        correction.Data()[5] = -1.0f;
        correction.Data()[10] = 0.5f;
        correction.Data()[14] = 0.5f;
        correction.Data()[15] = 1.0f;
        projection *= correction;
        // END TEMP ===================================================================

        IShader* currentShader = nullptr;
        if( _renderTable->StaticMeshCount > 0 ) {
            StaticMeshRenderReferenceData* ref = static_cast<StaticMeshRenderReferenceData*>( _renderTable->StaticMeshes[0]->GetReferenceData() );
            currentShader = ref->Material->GetShader();
            currentShader->ResetDescriptors( _currentImageIndex );

            // TODO: Don't create this every frame, save off locally
            GlobalUniformObject guo;
            guo.Projection = projection;
            guo.View = view;
            currentShader->SetGlobalUniform( currentCommandBuffer, guo, _currentImageIndex );
        }

        // Draw static meshes.        
        for( U32 i = 0; i < _renderTable->StaticMeshCount; ++i ) {
            StaticMeshEntityComponent* entity = _renderTable->StaticMeshes[i];
            const StaticMeshRenderReferenceData* ref = static_cast<StaticMeshRenderReferenceData*>( entity->GetReferenceData() );
            const VulkanBufferDataBlock* vertexBlock = _vertexBuffer->GetDataRangeByIndex( ref->VertexHeapIndex );
            const VulkanBufferDataBlock* indexBlock = _indexBuffer->GetDataRangeByIndex( ref->IndexHeapIndex );

            // Update the current descriptor set.
            IShader* shader = ref->Material->GetShader();
            if( shader != currentShader ) {
                currentShader->ResetDescriptors( _currentImageIndex );

                // TODO: Don't create this every frame, save off locally
                GlobalUniformObject guo;
                guo.Projection = projection;
                guo.View = view;
                currentShader->SetGlobalUniform( currentCommandBuffer, guo, _currentImageIndex );
            }

            // Bind the buffer to the graphics pipeline
            shader->BindPipeline( currentCommandBuffer );

            shader->SetModel( *entity->GetWorldMatrix() );

            // Update the descriptor for this object.
            shader->UpdateDescriptor( currentCommandBuffer, _currentImageIndex, i, ref->Material );


            shader->BindDescriptor( currentCommandBuffer, _currentImageIndex, i );

            // Bind vertex buffer
            _vertexBuffer->Bind( currentCommandBuffer, vertexBlock->Offset );

            // Bind index buffer
            _indexBuffer->Bind( currentCommandBuffer, indexBlock->Offset );

            // Make the draw call.
            //vkCmdDraw( _commandBuffers[imageIndex], 3, 1, 0, 0 );

            vkCmdDrawIndexed( currentCommandBuffer->Handle, (U32)indexBlock->ElementCount, 1, 0, 0, 0 );
        }

        // End render pass.
        currentCommandBuffer->EndRenderPass();

        // End recording
        currentCommandBuffer->End();

        return true;
    }

    const bool VulkanRendererBackend::Frame( const F32 deltaTime ) {

        // Check if a previous frame is using this image (i.e. its fence is being waited on)
        U8 swapchainFrameIndex = _swapchain->GetCurrentFrameIndex();
        if( _inFlightImageFences[swapchainFrameIndex] != VK_NULL_HANDLE ) {
            _inFlightImageFences[swapchainFrameIndex]->Wait( U64_MAX );
        }

        // Mark the image fence as in-use by this frame.
        _inFlightImageFences[swapchainFrameIndex] = _inFlightFences[swapchainFrameIndex];

        // Reset the fence for use on the next frame.
        _inFlightImageFences[swapchainFrameIndex]->Reset();

        // Ensure that the operation cannot begin until the image is available.
        _commandBuffers[_currentImageIndex]->AddWaitSemaphore( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, _imageAvailableSemaphores[swapchainFrameIndex] );

        // Submit the queue and wait for the operation to complete.
        _device->GraphicsQueue->Submit( _commandBuffers[_currentImageIndex], _inFlightFences[swapchainFrameIndex], 1, &_renderCompleteSemaphores[swapchainFrameIndex]->Handle, false );

        // Give the image back to the swapchain.
        _swapchain->Present( _device->GraphicsQueue, _device->PresentationQueue, _renderCompleteSemaphores[swapchainFrameIndex], _currentImageIndex );

        return true;
    }

    const bool VulkanRendererBackend::UploadMeshData( const MeshUploadData& data, StaticMeshRenderReferenceData* referenceData ) {

        _device->GraphicsQueue->WaitIdle();

        VulkanBufferDataBlock* vertBlock = _vertexBuffer->AllocateData( data.Vertices );
        VulkanBufferDataBlock* indexBlock = _indexBuffer->AllocateData( data.Indices );

        referenceData->VertexHeapIndex = vertBlock->HeapIndex;
        referenceData->IndexHeapIndex = indexBlock->HeapIndex;

        return true;
    }

    void VulkanRendererBackend::FreeMeshData( StaticMeshRenderReferenceData* referenceData ) {

        _device->WaitIdle();

        // Release the material reference.
        MaterialManager::Release( referenceData->Material->Name );

        // Release buffer data by index.
        _vertexBuffer->FreeDataRangeByIndex( referenceData->VertexHeapIndex );
        _indexBuffer->FreeDataRangeByIndex( referenceData->VertexHeapIndex );
    }

    void VulkanRendererBackend::SetRenderTable( WorldRenderableObjectTable* renderTable ) {
        _renderTable = renderTable;
    }

    void VulkanRendererBackend::OnEvent( const Event* event ) {
        switch( event->Type ) {
        case EventType::WINDOW_RESIZED:
            //const WindowResizedEvent wre = (const WindowResizedEvent)event;
            _framebufferResizeOccurred = true;
            if( _application && _device ) {
                _device->FramebufferSize = _application->GetApplicationWindow()->GetFramebufferExtent();
            }
            break;
        default:
            break;
        }
    }

    ITexture* VulkanRendererBackend::GetTexture( const TString& name, const TString& path ) {
        return new VulkanTexture( _device, name, path );
    }

    Extent2D VulkanRendererBackend::GetFramebufferExtent() {
        return _application->GetApplicationWindow()->GetFramebufferExtent();
    }

    IShader* VulkanRendererBackend::GetBuiltinMaterialShader( const MaterialType type ) {
        switch( type ) {
        default:
        case MaterialType::Unlit:
            return static_cast<IShader*>( _unlitShader );
        }
    }

    void VulkanRendererBackend::createInstance() {
        VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
        appInfo.apiVersion = VK_API_VERSION_1_2;
        appInfo.pApplicationName = "Epoch";
        appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );

        VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceCreateInfo.pApplicationInfo = &appInfo;

        // Extensions
        List<const char*> requiredExtensions;
        VulkanPlatform::GetRequiredExtensions( requiredExtensions );
        instanceCreateInfo.enabledExtensionCount = requiredExtensions.Size();
        instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.Data();

        // Validation layers
#ifdef VULKAN_USE_VALIDATION
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
#else
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.ppEnabledLayerNames = nullptr;
#endif

        // Create instance
        VK_CHECK( vkCreateInstance( &instanceCreateInfo, nullptr, &_instance ) );
    }

    void VulkanRendererBackend::createRenderPass() {

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

    void VulkanRendererBackend::createCommandBuffers() {
        U32 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        for( U32 i = 0; i < swapchainImageCount; ++i ) {
            VulkanCommandBuffer* cmdBuf = _device->CommandPool->AllocateCommandBuffer( true );
            _commandBuffers.push_back( cmdBuf );
        }
    }

    void VulkanRendererBackend::createSyncObjects() {
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

    void VulkanRendererBackend::cleanupSwapchain() {

        // Free the command buffers in the pool, but do not destroy the pool.
        U32 swapchainImageCount = _swapchain->GetSwapchainImageCount();
        for( U32 i = 0; i < swapchainImageCount; ++i ) {
            _device->CommandPool->FreeCommandBuffer( _commandBuffers[i] );
        }
        _commandBuffers.clear();

        VulkanRenderPassManager::DestroyRenderPass( _device, "RenderPass.Default" );
    }

    void VulkanRendererBackend::recreateSwapchain() {
        if( _recreatingSwapchain ) {
            return;
        }
        _recreatingSwapchain = true;
        Extent2D extent = _application->GetApplicationWindow()->GetFramebufferExtent();
        while( extent.Width == 0 || extent.Height == 0 ) {
            extent = _application->GetApplicationWindow()->GetFramebufferExtent();
        }

        // Cleanup and recreate swapchain.
        _device->RequerySupport();
        _swapchain->Recreate( extent.Width, extent.Height );

        // Cleanup the old swapchain.
        cleanupSwapchain();

        createRenderPass();
        _swapchain->RegenerateFramebuffers();

        createCommandBuffers();

        _recreatingSwapchain = false;
    }

    void VulkanRendererBackend::createBuffers() {

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