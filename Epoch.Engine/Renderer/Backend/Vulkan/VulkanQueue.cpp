
#include "VulkanDevice.h"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanUtilities.h"
#include "VulkanCommandBuffer.h"
#include "VulkanQueue.h"

#include <vector>

namespace Epoch {

    VulkanQueue::VulkanQueue( VulkanDevice* device, U32 index ) {
        _device = device;
        _index = index;
        vkGetDeviceQueue( _device->LogicalDevice, index, 0, &_handle );
    }

    VulkanQueue::~VulkanQueue() {
        _device = nullptr;
        _index = -1;
    }

    void VulkanQueue::Submit( VulkanCommandBuffer* commandBuffer, VulkanFence* fence, U32 signalSemaphoreCount, VkSemaphore* signalSemaphores, const bool waitForFinish ) {
        VkFence f = nullptr;
        if( fence ) {
            ASSERT( !fence->IsSignaled() );
            f = fence->GetHandle();
        }

        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

        // Command buffer
        submitInfo.commandBufferCount = 1;
        VkCommandBuffer buffers[1] = { commandBuffer->GetHandle() };
        submitInfo.pCommandBuffers = buffers;

        // Signal semaphores
        submitInfo.signalSemaphoreCount = signalSemaphoreCount;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // Wait semaphores
        std::vector<VkSemaphore> waitSemaphores;
        U32 waitSemaphoreCount = (U32)commandBuffer->GetWaitSemaphores().size();
        if( waitSemaphoreCount > 0 ) {
            waitSemaphores.clear();
            for( auto semaphore : commandBuffer->GetWaitSemaphores() ) {
                waitSemaphores.push_back( semaphore->GetHandle() );
            }

            submitInfo.waitSemaphoreCount = waitSemaphoreCount;
            submitInfo.pWaitSemaphores = waitSemaphores.data();
            submitInfo.pWaitDstStageMask = commandBuffer->GetWaitFlags().data();
        }

        VK_CHECK( vkQueueSubmit( _handle, 1, &submitInfo, f ) );

        commandBuffer->_state = CommandBufferState::Submitted;
        commandBuffer->UpdateSubmitted();

        if( waitForFinish ) {
            WaitIdle();
        }
    }

    void VulkanQueue::WaitIdle() {
        VK_CHECK( vkQueueWaitIdle( _handle ) );
    }
}