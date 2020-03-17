#pragma once

#include "../../../Types.h"
#include "../../../Events/IEventHandler.h"
#include "../IRendererBackend.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Epoch {

    struct VulkanSwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentationModes;
    };

    class Platform;
    class VulkanImage;
    class VulkanVertex3DBuffer;
    class VulkanIndexBuffer;
    class VulkanImage;
    class VulkanUniformBuffer;

    class VulkanRenderer final : public IRendererBackend, IEventHandler {
    public:
        VulkanRenderer( Platform* platform );
        ~VulkanRenderer();

        VkDevice GetDeviceHandle() { return _device; }
        VkPhysicalDevice GetPhysicalDeviceHandle() { return _physicalDevice; }
        VkQueue GetGraphicsQueue() { return _graphicsQueue; }
        VkCommandPool GetCommandPool() { return _commandPool; }

        const bool Initialize() override;

        const bool Frame( const F32 deltaTime ) override;

        void OnEvent( const Event& event ) override;

        /**
         * Allocates and begins recording of a single use command buffer. This is useful for copying
         * from a staging buffer or for transitioning image layouts. Note that a call to EndSingleUseCommandBuffer
         * should be made when ready to submit for execution.
         *
         * @return A pointer to the newly-allocated command buffer.
         */
        VkCommandBuffer AllocateAndBeginSingleUseCommandBuffer();

        /**
         * Ends recording of the given command buffer and submits it for execution. This function
         * waits until the execution of the queue is complete, then frees it before returning.
         */
        void EndSingleUseCommandBuffer( VkCommandBuffer commandBuffer );

    private:
        VkPhysicalDevice selectPhysicalDevice();
        const bool physicalDeviceMeetsRequirements( VkPhysicalDevice physicalDevice );
        void detectQueueFamilyIndices( VkPhysicalDevice physicalDevice, I32* graphicsQueueIndex, I32* presentationQueueIndex );
        VulkanSwapchainSupportDetails querySwapchainSupport( VkPhysicalDevice physicalDevice );
        void createLogicalDevice( std::vector<const char*>& requiredValidationLayers );
        void createShader( const char* name );
        char* readShaderFile( const char* filename, const char* shaderType, U64* fileSize );
        void createSwapchain();
        void createSwapchainImagesAndViews();
        void createRenderPass();
        void createGraphicsPipeline();
        void createCommandPool();
        void createDepthResources();
        void createFramebuffers();
        void createUniformBuffers();
        void updateUniformBuffers( U32 currentImageIndex );
        void createDescriptorSetLayout();
        void createDescriptorPool();
        void createDescriptorSets();
        void updateDescriptorSet( U64 descriptorSetIndex, VulkanImage* textureImage, VkSampler sampler );
        void createCommandBuffers();
        void createSyncObjects();
        void cleanupSwapchain();
        void recreateSwapchain();

        // Asset loading temp
        void createBuffers();
        void createTextureImageAndView( const char* path, VulkanImage** textureImage );
        void createTextureSampler( VkSampler* sampler );
    private:
        Platform* _platform;

        VkInstance _instance;

        VkDebugUtilsMessengerEXT _debugMessenger;

        VkPhysicalDevice _physicalDevice;
        VkDevice _device; // Logical device 
        I32 _graphicsFamilyQueueIndex;
        I32 _presentationFamilyQueueIndex;
        VkQueue _graphicsQueue;
        VkQueue _presentationQueue;

        VkSurfaceKHR _surface;

        U64 _shaderStageCount;
        std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;

        VkSurfaceFormatKHR _swapchainImageFormat;
        VkExtent2D _swapchainExtent;
        VkSwapchainKHR _swapchain;

        std::vector<VkImage> _swapchainImages;
        std::vector<VkImageView> _swapchainImageViews;

        VkRenderPass _renderPass;
        VkPipelineLayout _pipelineLayout;
        VkPipeline _pipeline;

        VkCommandPool _commandPool;

        // Depth image.
        VkFormat _depthFormat;
        VulkanImage* _depthImage;

        // Framebuffers - one each for front/back buffer
        bool _framebufferResizeOccurred = false;
        bool _recreatingSwapchain = false;
        std::vector<VkFramebuffer> _swapchainFramebuffers;

        // Command buffers
        std::vector<VkCommandBuffer> _commandBuffers;

        // Sync objects
        U8 _currentFrameIndex;
        const U8 _maxFramesInFlight = 2;
        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderCompleteSemaphores;
        std::vector<VkFence> _inFlightFences;
        std::vector<VkFence> _inFlightImageFences;

        // Descriptor pools/sets
        VkDescriptorPool _descriptorPool;
        VkDescriptorSetLayout _descriptorSetLayout;
        std::vector<VkDescriptorSet> _descriptorSets;

        // 1 per swap chain image.
        std::vector<VulkanUniformBuffer*> _uniformBuffers;


        // Asset load temp
        VulkanVertex3DBuffer* _vertexBuffer;
        VulkanIndexBuffer* _indexBuffer;

        // textures
        U64 _updatesTemp = 0;
        U64 _currentTextureIndex = 0;
        VulkanImage* _textureImages[2];
        VkSampler _textureSamplers[2];
    };
}
