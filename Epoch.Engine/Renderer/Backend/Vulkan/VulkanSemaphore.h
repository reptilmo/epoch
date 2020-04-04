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
         * Creates a Vulkan Semaphore object.
         *
         * @param device The device to which this semaphore belongs.
         */
        VulkanSemaphore( VulkanDevice* device );

        /**
         * Default destructor.
         */
        ~VulkanSemaphore();

        /**
         * Retrieves the internal handle of this semaphore.
         */
        VkSemaphore GetHandle() { return _handle; }

    private:
        VulkanDevice* _device;
        VkSemaphore _handle;
    };
}