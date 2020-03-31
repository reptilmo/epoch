#pragma once

#include <vulkan/vulkan.h>

namespace Epoch {

    class VulkanDevice;

    /**
     * Used for synchronization between CPU and GPU events.
     */
    class VulkanFence {
    public:

        /**
         * Creates a Vulkan Fence object.
         * 
         * @param device The device to which this fence belongs.
         * @param createSignaled Indicates if this fence should be created in a signaled state.
         */
        VulkanFence( VulkanDevice* device, const bool createSignaled = false );

        /**
         * Default destructor.
         */
        ~VulkanFence();

        /**
         * Waits for this fence to complete.
         * 
         * @param timeoutNS The amount of time in nanoseconds to wait before timing out.
         */
        const bool Wait( U64 timeoutNS );

        /**
         * Reset this fence to its default state.
         */
        void Reset();

        /**
         * Retrieves the internal handle of this fence.
         */
        VkFence GetHandle() { return _handle; }

        const bool IsSignaled() const { return _state == VulkanFenceState::Signaled; }

        enum class VulkanFenceState {

            // Initial state, unless created signaled.
            NotReady,

            // Post-GPU work complete.
            Signaled
        };

    private:
        VulkanDevice* _device;
        VkFence _handle;
        VulkanFenceState _state;
    };
}