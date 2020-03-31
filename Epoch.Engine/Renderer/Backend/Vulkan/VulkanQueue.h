#pragma once

#include <vulkan/vulkan.h>

namespace Epoch {

    class VulkanDevice;
    class VulkanCommandBuffer;
    class VulkanFence;

    class VulkanQueue {
    public:
        VulkanQueue( VulkanDevice* device, U32 index );
        ~VulkanQueue();

        /**
         * Submits the provided command buffer to this queue for execution.
         * 
         * @param commandBuffer The command buffer to be submitted for execution.
         * @param fence The fence to be used for the submit operation.
         * @param waitInfos A collection of wait information, each including a semaphore and corresponding wait stage, which must be complete before this submission can commence.
         *
         * @param signalSemaphore The semaphore to be signaled when this operation completes.
         * @param waitForFinish Indicates if this function should wait until the operation's completion before returning. Default: false.
         */
        void Submit( VulkanCommandBuffer* commandBuffer, VulkanFence* fence, U32 signalSemaphoreCount = 0, VkSemaphore* signalSemaphores = nullptr, const bool waitForFinish = false );

        /**
         * Waits until this queue is idle before returning.
         */
        void WaitIdle();

        VkQueue GetHandle() { return _handle; }
        const U32 GetIndex() const { return _index; }

    private:
        VulkanDevice* _device;
        U32 _index;
        VkQueue _handle;
    };
}
