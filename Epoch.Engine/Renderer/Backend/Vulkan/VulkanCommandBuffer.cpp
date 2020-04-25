
#include "../../../Memory/Memory.h"
#include "VulkanUtilities.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanRenderPass.h"
#include "VulkanSemaphore.h"
#include "VulkanCommandBuffer.h"


namespace Epoch {

    VulkanCommandBuffer::VulkanCommandBuffer( VkCommandBuffer handle, VulkanCommandPool* owner, const bool isPrimary ) {
        Handle = handle;
        _owner = owner;
        _isPrimary = isPrimary;
        _state = CommandBufferState::Ready;
    }

    VulkanCommandBuffer::~VulkanCommandBuffer() {
        Handle = nullptr;
        _owner = nullptr;
        _isPrimary = false;
        _state = CommandBufferState::NotAllocated;

        if( _waitFlags ) {
            TMemory::Free( _waitFlags );
            _waitFlags = nullptr;
        }
        _waitFlagAllocatedCount = 0;
        _waitFlagCount = 0;

        if( _waitSemaphores ) {
            TMemory::Free( _waitSemaphores );
            _waitSemaphores = nullptr;
        }
        _waitSemaphoreAllocatedCount = 0;
        _waitSemaphoreCount = 0;
    }

    void VulkanCommandBuffer::Begin( const bool isSingleUse, const bool isRenderPassContinue, const bool isSimultaneousUse ) {

        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = 0;
        if( isSingleUse ) {
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        }
        if( isRenderPassContinue ) {
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        }
        if( isSimultaneousUse ) {
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        }
        beginInfo.pInheritanceInfo = nullptr;

        VK_CHECK( vkBeginCommandBuffer( Handle, &beginInfo ) );

        _state = CommandBufferState::Recording;
    }

    void VulkanCommandBuffer::End() {
        VK_CHECK( vkEndCommandBuffer( Handle ) );
        _state = CommandBufferState::RecordingEnded;
    }

    void VulkanCommandBuffer::BeginRenderPass( const RenderPassClearInfo& clearInfo, VkFramebuffer framebuffer, VulkanRenderPass* renderPass ) {
        _currentRenderPass = renderPass;
        _currentRenderPass->Begin( clearInfo, framebuffer, this );
        _state = CommandBufferState::InRenderPass;
    }

    void VulkanCommandBuffer::EndRenderPass() {
        _currentRenderPass->End( this );
        _state = CommandBufferState::Recording;
    }

    void VulkanCommandBuffer::AddWaitSemaphore( VkPipelineStageFlags waitFlags, VulkanSemaphore* waitSemaphore ) {

        // Wait flags - Allocate more space if need be.
        if( _waitFlagAllocatedCount <= _waitFlagCount ) {
            VkPipelineStageFlags* temp = static_cast<VkPipelineStageFlags*>( TMemory::Allocate( sizeof( VkPipelineStageFlags ) * ( _waitFlagCount + 1 ) ) );
            if( _waitFlags ) {
                TMemory::Memcpy( temp, _waitFlags, _waitFlagCount );
                TMemory::Free( _waitFlags );
            }
            _waitFlags = temp;
            _waitFlagAllocatedCount++;
        }
        _waitFlags[_waitFlagCount] = waitFlags;
        _waitFlagCount++;

        // Wait semaphores - Allocate more space if need be.
        if( _waitSemaphoreAllocatedCount <= _waitSemaphoreCount ) {
            VulkanSemaphore** temp = static_cast<VulkanSemaphore**>( TMemory::Allocate( sizeof( VulkanSemaphore* ) * ( _waitSemaphoreCount + 1 ) ) );
            if( _waitSemaphores ) {
                TMemory::Memcpy( temp, _waitSemaphores, _waitSemaphoreCount );
                TMemory::Free( _waitSemaphores );
            }
            _waitSemaphores = temp;
            _waitSemaphoreAllocatedCount++;
        }
        _waitSemaphores[_waitSemaphoreCount] = waitSemaphore;
        _waitSemaphoreCount++;
    }

    void VulkanCommandBuffer::UpdateSubmitted() {
        _waitSemaphoreCount = 0;
        _waitFlagCount = 0;
    }

    void VulkanCommandBuffer::Reset() {
        _waitSemaphoreCount = 0;
        _waitFlagCount = 0;
    }
}