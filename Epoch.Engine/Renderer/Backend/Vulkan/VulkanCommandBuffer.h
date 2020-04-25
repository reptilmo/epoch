#pragma once

#include <vulkan/vulkan.h>

#include "../../../Types.h"
#include "../../ICommandBuffer.h"
#include "../../../Defines.h"

namespace Epoch {

    class VulkanCommandPool;
    class VulkanRenderPass;
    class VulkanSemaphore;
    struct RenderPassClearInfo;

    enum class CommandBufferState {
        Ready,
        Recording,
        InRenderPass,
        RecordingEnded,
        Submitted,
        NotAllocated
    };

    /**
     * Represents a Vulkan-specific implementation of a command buffer, which can have commands recorded to it and then be submitted to
     * a queue for execution.
     */
    class VulkanCommandBuffer : public ICommandBuffer {
        friend class VulkanQueue;
    public:
        VkCommandBuffer Handle;
    public:
        VulkanCommandBuffer( VkCommandBuffer handle, VulkanCommandPool* owner, const bool isPrimary );
        ~VulkanCommandBuffer();

        /**
         * Begins recording of this command buffer.
         *
         * @param isSingleUse Specifies this command buffer will only be used once and will be reset and recorded again between each submission. Default: false
         * @param isRenderPassContinue Specifies that a secondary buffer is considered to be entirely inside a render pass. Default: false
         * @param isSimultaneousUse Specifies that the buffer can be resubmitted to a queue while it is in pending state and recorded into multiple primary buffers. Default: false
         */
        void Begin( const bool isSingleUse = false, const bool isRenderPassContinue = false, const bool isSimultaneousUse = false );

        /**
         * Ends recording of this command buffer. From here, the buffer may be submitted to a queue for execution.
         */
        void End();

        void BeginRenderPass( const RenderPassClearInfo& clearInfo, VkFramebuffer framebuffer, VulkanRenderPass* renderPass );

        void EndRenderPass();

        void AddWaitSemaphore( VkPipelineStageFlags waitFlags, VulkanSemaphore* waitSemaphore );
        void UpdateSubmitted();
        void Reset();

        const U32 GetWaitSemaphoreCount() const { return _waitSemaphoreCount; }
        VulkanSemaphore* GetWaitSemaphore( const U32 index ) { return _waitSemaphores[index]; }
        const VkPipelineStageFlags* GetWaitFlags() const { return _waitFlags; }

        FORCEINLINE const bool IsInRenderPass() const {
            return _state == CommandBufferState::InRenderPass;
        }

        FORCEINLINE const bool IsRecording() const {
            return _state == CommandBufferState::Recording || _state == CommandBufferState::InRenderPass;
        }

        FORCEINLINE const bool HasRecordingEnded() const {
            return _state == CommandBufferState::RecordingEnded;
        }

        FORCEINLINE const bool IsSubmitted() const {
            return _state == CommandBufferState::Submitted;
        }

        FORCEINLINE const bool IsAllocated() const {
            return _state != CommandBufferState::NotAllocated;
        }

    private:
        bool _isPrimary;
        VulkanCommandPool* _owner;
        CommandBufferState _state;
        VulkanRenderPass* _currentRenderPass;

        U32 _waitSemaphoreCount = 0;
        U32 _waitSemaphoreAllocatedCount = 0;
        VulkanSemaphore** _waitSemaphores = nullptr;

        U32 _waitFlagCount = 0;
        U32 _waitFlagAllocatedCount = 0;
        VkPipelineStageFlags* _waitFlags = nullptr;
    };
}