#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "../../../Types.h"

namespace Epoch {

    class VulkanDevice;
    class VulkanTexture;

    class VulkanSwapchain {
    public:
        VkSurfaceFormatKHR ImageFormat;
        VkExtent2D Extent;
        const U8 MaxFramesInFlight = 2;
    public:
        VulkanSwapchain( VulkanDevice* device, VkSurfaceKHR surface, void* windowHandle, const U32 width, const U32 height );
        ~VulkanSwapchain();

        void Recreate( const U32 width, const U32 height );
        void RegenerateFramebuffers();

        void Present( VkQueue graphicsQueue, VkQueue presentQueue, VkSemaphore renderCompleteSemaphore, const U32 presentImageIndex );

        const U32 GetSwapchainImageCount() const { return (U32)_swapchainImages.size(); }
        VkSwapchainKHR GetHandle() { return _swapchain; }

        VkImageView GetView( U64 index ) { return _swapchainImageViews[index]; }

        VkFramebuffer GetFramebuffer( U64 index ) { return _swapchainFramebuffers[index]; }

        const U8 GetCurrentFrameIndex() const { return _currentFrameIndex; }
    private:
        void destroyInternal();
        void createInternal();

        
    private:
        VulkanDevice* _device;
        VkSurfaceKHR _surface;

        
        U32 _currentFrameIndex;
        VkSwapchainKHR _swapchain;

        std::vector<VkImage> _swapchainImages;
        std::vector<VkImageView> _swapchainImageViews;

        // Depth image.
        VkFormat _depthFormat;
        VulkanTexture* _depthAttachment;

        // Framebuffers - one each for front/back buffer        
        std::vector<VkFramebuffer> _swapchainFramebuffers;
        
    };
}