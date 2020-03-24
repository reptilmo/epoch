#pragma once

#include "../../../Types.h"
#include "../../../Events/IEventHandler.h"
#include "../IRendererBackend.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Epoch {

    

    class ITexture;

    class Platform;    
    class VulkanVertex3DBuffer;
    class VulkanIndexBuffer;
    class VulkanImage;
    class VulkanTexture;
    class VulkanUniformBuffer;
    class VulkanDebugger;
    class VulkanDevice;

    class VulkanRenderer final : public IRendererBackend, IEventHandler {
    public:
        VulkanRenderer( Platform* platform );
        ~VulkanRenderer();

        VulkanDevice* GetDevice() { return _device; }

        const bool Initialize() override;

        const bool Frame( const F32 deltaTime ) override;

        void OnEvent( const Event& event ) override;

        ITexture* GetTexture( const char* path );

    private:
        void createShader( const char* name );
        char* readShaderFile( const char* filename, const char* shaderType, U64* fileSize );
        void createSwapchain();
        void createSwapchainImagesAndViews();
        void createRenderPass();
        void createGraphicsPipeline();
        
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
        void createTextureSampler( VkSampler* sampler );
    private:
        Platform* _platform;

        VkInstance _instance;
        VulkanDevice* _device;

        VulkanDebugger* _debugger;

        VkSurfaceKHR _surface;

        U64 _shaderStageCount;
        std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
        VkShaderModule _vertexShaderModule;
        VkShaderModule _fragmentShaderModule;

        VkSurfaceFormatKHR _swapchainImageFormat;
        VkExtent2D _swapchainExtent;
        VkSwapchainKHR _swapchain;

        std::vector<VkImage> _swapchainImages;
        std::vector<VkImageView> _swapchainImageViews;

        VkRenderPass _renderPass;
        VkPipelineLayout _pipelineLayout;
        VkPipeline _pipeline;

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
        VulkanTexture* _textures[2];
        VkSampler _textureSamplers[2];
    };
}
