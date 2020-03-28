#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "../../../Types.h"

namespace Epoch {

    class VulkanDevice;
    class VulkanTexture;
    class VulkanFence;
    class VulkanSemaphore;

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

        /**
         * Attempt to acquire the next image index. If this returns false, the caller should abort and wait for the next frame.
         *
         * @param timeoutNS The amount of time in nanoseconds to wait for the operation to complete.
         * @param imageAvailableSemaphore A pointer to the semaphore to wait on regarding image availability.
         * @param fence A pointer to a fence to be waited on. Pass nullptr if not used.
         * @param imageIndex A pointer to an image index, which gets updated upon operation completion.
         *
         * @returns True if rendering should continue; otherwise false. If false, the caller should abort and wait for the next frame.
         */
        const bool AcquireNextImageIndex( const U64 timeoutNS, VulkanSemaphore* imageAvailableSemaphore, VulkanFence* fence, U32* imageIndex );

        void Present( VkQueue graphicsQueue, VkQueue presentQueue, VulkanSemaphore* renderCompleteSemaphore, const U32 presentImageIndex );

        const U32 GetSwapchainImageCount() const { return (U32)_swapchainImages.size(); }
        VkSwapchainKHR GetHandle() { return _handle; }

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
        VkSwapchainKHR _handle;

        std::vector<VkImage> _swapchainImages;
        std::vector<VkImageView> _swapchainImageViews;

        // Depth image.
        VkFormat _depthFormat;
        VulkanTexture* _depthAttachment;

        // Framebuffers - one each for front/back buffer        
        std::vector<VkFramebuffer> _swapchainFramebuffers;

    };
}