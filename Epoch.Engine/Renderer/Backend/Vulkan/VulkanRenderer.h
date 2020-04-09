#pragma once

#include "../../../Types.h"
#include "../../../Events/IEventHandler.h"
#include "../IRendererBackend.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Epoch {

    struct MeshUploadData;

    class ITexture;
    class TString;
    class IUnlitShader;

    class Platform;    
    class VulkanVertex3DBuffer;
    class VulkanIndexBuffer;
    class VulkanImage;
    class VulkanTexture;
    class VulkanUniformBuffer;
    class VulkanDebugger;
    class VulkanDevice;
    class VulkanSwapchain;
    class VulkanFence;
    class VulkanSemaphore;
    class VulkanCommandBuffer;
    class VulkanGraphicsPipeline;
    class VulkanTextureSampler;
    class VulkanShader;

    class VulkanRenderer final : public IRendererBackend, IEventHandler {
    public:
        VulkanRenderer( Platform* platform );
        ~VulkanRenderer();

        VulkanDevice* GetDevice() { return _device; }

        const bool Initialize() override;

        /**
         * Destroys this renderer, releasing all of its resources.
         */
        void Destroy() override;

        /**
         * Performs operations required for the next frame render.
         *
         * @param deltaTime The amount of time in seconds since the last frame.
         *
         * @returns True if Frame() should be called; otherwise false.
         */
        const bool PrepareFrame( const F32 deltaTime ) override;

        /**
         * Processes a single frame.
         *
         * @param deltaTime The amount of time in seconds since the last frame.
         *
         * @returns True on success, false on failure. Returning false crashes the application.
         */
        const bool Frame( const F32 deltaTime ) override;

        const bool UploadMeshData( const MeshUploadData& data, MeshRendererReferenceData* referenceData ) override;

        void FreeMeshData( const U64 index ) override;

        void AddToFrameRenderList( const MeshRendererReferenceData* referenceData ) override;

        void OnEvent( const Event* event ) override;

        ITexture* GetTexture( const TString& name, const TString& path ) override;

        Extent2D GetFramebufferExtent();

        IShader* GetBuiltinMaterialShader( const MaterialType type );

    private:
        void createInstance();
        void createRenderPass();
        //void createGraphicsPipeline();
        
        void createUniformBuffers();
        void updateUniformBuffers( U32 currentImageIndex );
        /*void createDescriptorSetLayout();
        void createDescriptorPool();
        void createDescriptorSets();*/
        //void updateDescriptorSet( U64 descriptorSetIndex, VulkanImage* textureImage, VulkanTextureSampler* sampler );
        void createCommandBuffers();
        void createSyncObjects();
        void cleanupSwapchain();
        void recreateSwapchain();

        // Asset loading temp
        void createBuffers();
    private:
        U32 _currentImageIndex;
        Platform* _platform;

        std::vector<const char*> _requiredValidationLayers;

        VkInstance _instance;
        VulkanDevice* _device;

        VulkanDebugger* _debugger;

        VkSurfaceKHR _surface;

        IShader* _unlitShader;


        VulkanSwapchain* _swapchain;
        bool _recreatingSwapchain = false;
        bool _framebufferResizeOccurred = false;

        // Command buffers
        std::vector<VulkanCommandBuffer*> _commandBuffers;

        

        // Sync objects
        std::vector<VulkanSemaphore*> _imageAvailableSemaphores;
        std::vector<VulkanSemaphore*> _renderCompleteSemaphores;
        std::vector<VulkanFence*> _inFlightFences;
        std::vector<VulkanFence*> _inFlightImageFences;

        // 1 per swap chain image.
        std::vector<VulkanUniformBuffer*> _uniformBuffers;

        // Asset load temp
        VulkanVertex3DBuffer* _vertexBuffer;
        VulkanIndexBuffer* _indexBuffer;

        std::vector<const MeshRendererReferenceData*> _currentRenderList;
    };
}
