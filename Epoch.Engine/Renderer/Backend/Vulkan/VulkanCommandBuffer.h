#pragma once

#include <vulkan/vulkan.h>

#include "../../../Defines.h"

namespace Epoch {

    class VulkanCommandPool;
    class VulkanRenderPass;
    struct RenderPassClearInfo;

    class VulkanCommandBuffer {
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

        VkCommandBuffer GetHandle() { return _handle; }

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

        enum class CommandBufferState {
            Ready,
            Recording,
            InRenderPass,
            RecordingEnded,
            Submitted,
            NotAllocated
        };

    private:
        bool _isPrimary;
        VkCommandBuffer _handle;
        VulkanCommandPool* _owner;
        CommandBufferState _state;
        VulkanRenderPass* _currentRenderPass;
    };
}