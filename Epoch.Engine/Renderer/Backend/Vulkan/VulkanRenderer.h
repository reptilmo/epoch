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

    class VulkanRenderer : public IRendererBackend, IEventHandler {
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
        void createDescriptorPool();
        void createDescriptorSets();
        void createCommandBuffers();
        void createSyncObjects();
        void cleanupSwapchain();
        void recreateSwapchain();

        // Asset loading temp
        void createBuffers();
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


        // Asset load temp
        VulkanVertex3DBuffer* _vertexBuffer;
        VulkanIndexBuffer* _indexBuffer;
    };
}
