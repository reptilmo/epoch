
#include "VulkanUtilities.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandBuffer.h"

namespace Epoch {

    VulkanCommandBuffer::VulkanCommandBuffer( VkCommandBuffer handle, VulkanCommandPool* owner, const bool isPrimary ) {
        _handle = handle;
        _owner = owner;
        _isPrimary = isPrimary;
        _state = CommandBufferState::Ready;
    }

    VulkanCommandBuffer::~VulkanCommandBuffer() {
        _handle = nullptr;
        _owner = nullptr;
        _isPrimary = false;
        _state = CommandBufferState::NotAllocated;
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

        VK_CHECK( vkBeginCommandBuffer( _handle, &beginInfo ) );

        _state = CommandBufferState::Recording;
    }

    void VulkanCommandBuffer::End() {
        VK_CHECK( vkEndCommandBuffer( _handle ) );
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
        _waitFlags.push_back( waitFlags );
        _waitSemaphores.push_back( waitSemaphore );
    }

    void VulkanCommandBuffer::UpdateSubmitted() {
        _submittedWaitSemaphores = _waitSemaphores;
        _waitSemaphores.clear();
        _waitFlags.clear();
    }

    void VulkanCommandBuffer::Reset() {
        _waitFlags.clear();
        _waitSemaphores.clear();
        _submittedWaitSemaphores.clear();
    }
}