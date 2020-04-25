#pragma once

#include <vulkan/vulkan.h>

namespace Epoch {
    
    class VulkanDevice;

    /**
     * Used for synchronization between GPU events.
     */
    class VulkanSemaphore {
    public:

        /**
         * The internal handle of this semaphore.
         */
        VkSemaphore Handle;

        bool IsSignaled = false;
    public:

        /**
         * Creates a Vulkan Semaphore object.
         *
         * @param device The device to which this semaphore belongs.
         */
        VulkanSemaphore( VulkanDevice* device, const bool isSignaled = false );

        /**
         * Default destructor.
         */
        ~VulkanSemaphore();

    private:
        VulkanDevice* _device;
    };
}