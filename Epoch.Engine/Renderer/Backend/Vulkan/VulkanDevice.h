#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "../../../Types.h"

namespace Epoch {

    struct VulkanSwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentationModes;
    };

    /**
     * Represents both the physical and logical device for Vulkan, as well as any device-specific
     * information they contain.
     */
    class VulkanDevice {
    public:

        /**
         * The current Vulkan implementation's representation of the physical device (GPU).
         */
        VkPhysicalDevice PhysicalDevice = nullptr;

        /**
         * The application's view of the physical device.
         */
        VkDevice LogicalDevice = nullptr;

        /**
         * The index for the graphics queue family.
         */
        I32 GraphicsFamilyQueueIndex;

        /**
         * The index for the presentation queue family.
         */
        I32 PresentationFamilyQueueIndex;

        /**
         * The queue used for graphics pipeline commands.
         */
        VkQueue GraphicsQueue;

        /**
         * The queue used for presentation commands.
         */
        VkQueue PresentationQueue;

        /**
         * Contains swapchain support details.
         */
        VulkanSwapchainSupportDetails SwapchainSupport;

        /**
         * The command pool associated with this device.
         */
        VkCommandPool CommandPool;

        /**
         *The format to be used for depth images/attachments.
         */
        VkFormat DepthFormat;

        /**
         * TODO: Stop-gap until platform is removed.
         */
        Extent2D FramebufferSize;

    public:
        /**
         * Creates a new VulkanDevice.
         *
         * @param instance The Vulkan instance.
         * @param requiredValidationLayers The names of required validation layers. This should be empty on non-debug builds.
         * @param surface The surface which this device will interact.
         */
        VulkanDevice( VkInstance instance, const std::vector<const char*>& requiredValidationLayers, VkSurfaceKHR surface );

        /**
         * Default destructor.
         */
        ~VulkanDevice();

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

        /**
         * Re-query device support details.
         */
        void RequerySupport();

    private:
        void selectPhysicalDevice();
        const bool physicalDeviceMeetsRequirements( VkPhysicalDevice physicalDevice, const VkPhysicalDeviceProperties* properties, const VkPhysicalDeviceFeatures* features );
        void detectQueueFamilyIndices( VkPhysicalDevice physicalDevice, I32* graphicsQueueIndex, I32* presentationQueueIndex );
        void querySwapchainSupport( VkPhysicalDevice physicalDevice );

        void createLogicalDevice( const std::vector<const char*>& requiredValidationLayers );

        void createQueues();

        void createCommandPool();
        const bool detectDepthFormat();
    private:
        VkInstance _instance;
        VkSurfaceKHR _surface;
    };
}