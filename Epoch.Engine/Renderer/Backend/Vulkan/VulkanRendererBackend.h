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

#if _DEBUG
    class VulkanDebugger;
#endif
    class VulkanDevice;
    class VulkanSwapchain;
    class VulkanFence;
    class VulkanSemaphore;
    class VulkanCommandBuffer;
    class VulkanShader;

    /**
     * The Vulkan-specific renderer backend. Implements IRendererBackend and is called by the front-end via that interface.
     * Contains all logic required to stand up the back-end via Vulkan-specific API calls.
     */
    class VulkanRendererBackend final : public IRendererBackend, IEventHandler {
    public:

        /**
         * Creates a new renderer backend.
         * 
         * @param platform A pointer to the platform layer.
         */
        VulkanRendererBackend( Platform* platform );

        /**
         * Default destructor.
         */
        ~VulkanRendererBackend();

        /**
         * Obtains a pointer to the Vulkan Device.
         */
        VulkanDevice* GetDevice() { return _device; }

        /**
         * Indicates if this backend is shut down.
         */
        const bool IsShutdown() const { return _isShutDown; }

        /**
         * Initializes this renderer.
         *
         * @returns True if successful; otherwise false.
         */
        const bool Initialize() override;

        /**
         * Flags this renderer as shut down and begins the shutdown process. Must be invoked before Destroy or delete.
         */
        void Shutdown() override;

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

        /**
         * Uploads the provided mesh data to this backend and sets the render API reference data in the provided referenceData object.
         *
         * @param data A reference to the data to be uploaded.
         * @param referenceData A pointer to the reference data object which will hold the API reference info.
         *
         * @returns True if successful; otherwise false.
         */
        const bool UploadMeshData( const MeshUploadData& data, MeshRendererReferenceData* referenceData ) override;

        /**
         * Frees mesh data using the provided reference data.
         *
         * @param referenceData A pointer to the reference data object whose data should be released.
         */
        void FreeMeshData( MeshRendererReferenceData* referenceData ) override;

        /**
         * Adds the object referenced by the provided reference data to a list of objects to be rendered in the next frame.
         *
         * @param referenceData A pointer to the reference data object to be rendered.
         */
        void AddToFrameRenderList( const MeshRendererReferenceData* referenceData ) override;

        /**
         * Processes the given event.
         *
         * @param event The event to be processed.
         */
        void OnEvent( const Event* event ) override;

        /**
         * Returns a new texture from this renderer. This is always a creation request, as the front end
         * should have already determined if a texture with the given name exists at this point.
         *
         * @param name The name of the texture.
         * @param path The file path of the texture.
         *
         * @returns A pointer to the newly-created texture.
         */
        ITexture* GetTexture( const TString& name, const TString& path ) override;

        Extent2D GetFramebufferExtent();

        /**
         * Obtains a pointer to a builtin shader of the given type. Only one builtin shader of each type exists.
         *
         * @param type The type whose shader to retrieve.
         *
         * @returns A pointer to the builtin shader. Can return nullptr if not found.
         */
        IShader* GetBuiltinMaterialShader( const MaterialType type );

    private:
        void createInstance();
        void createRenderPass();
        void createCommandBuffers();
        void createSyncObjects();
        void cleanupSwapchain();
        void recreateSwapchain();
        void createBuffers();
    private:
        bool _isShutDown = false;
        U32 _currentImageIndex = 0;
        Platform* _platform = nullptr;

        std::vector<const char*> _requiredValidationLayers;

        VkInstance _instance = nullptr;
        VulkanDevice* _device = nullptr;

#if _DEBUG
        VulkanDebugger* _debugger = nullptr;
#endif
        // The surface (within the window) which this renderer will render to.
        VkSurfaceKHR _surface = nullptr;

        // Shaders
        IShader* _unlitShader = nullptr;

        // Swapchain.
        VulkanSwapchain* _swapchain = nullptr;
        bool _recreatingSwapchain = false;
        bool _framebufferResizeOccurred = false;

        // Command buffers, 1 per frame
        std::vector<VulkanCommandBuffer*> _commandBuffers;

        // Sync objects
        std::vector<VulkanSemaphore*> _imageAvailableSemaphores;
        std::vector<VulkanSemaphore*> _renderCompleteSemaphores;
        std::vector<VulkanFence*> _inFlightFences;
        std::vector<VulkanFence*> _inFlightImageFences;

        // Buffers
        VulkanVertex3DBuffer* _vertexBuffer = nullptr;
        VulkanIndexBuffer* _indexBuffer = nullptr;

        // A list of objects to be rendered in the current frame.
        // TODO: This should probably be an object pool instead for performance reasons.
        std::vector<const MeshRendererReferenceData*> _currentRenderList;
    };
}
