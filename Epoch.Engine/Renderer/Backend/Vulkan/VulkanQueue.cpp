
#include "../../../Memory/Memory.h"
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
        submitInfo.pCommandBuffers = &commandBuffer->Handle;

        // Signal semaphores
        submitInfo.signalSemaphoreCount = signalSemaphoreCount;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // Wait semaphores
        U32 waitSemaphoreCount = commandBuffer->GetWaitSemaphoreCount();
        VkSemaphore* waitSemaphoreHandles = static_cast<VkSemaphore*>( TMemory::Allocate( sizeof( VkSemaphore ) * waitSemaphoreCount ) );
        if( waitSemaphoreCount > 0 ) {
            for( U32 i = 0; i < waitSemaphoreCount; ++i ) {
                waitSemaphoreHandles[i] = commandBuffer->GetWaitSemaphore( i )->Handle;
            }

            submitInfo.waitSemaphoreCount = waitSemaphoreCount;
            submitInfo.pWaitSemaphores = waitSemaphoreHandles;
            submitInfo.pWaitDstStageMask = commandBuffer->GetWaitFlags();
        }

        VkResult result = vkQueueSubmit( _handle, 1, &submitInfo, f );
        VK_CHECK( result );

        TMemory::Free( waitSemaphoreHandles );

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