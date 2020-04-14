#pragma once


namespace Epoch {

    /**
     * Represents a command buffer, which can have commands recorded to it and then be submitted to
     * a queue for execution.
     */
    class ICommandBuffer {
    public:

        /**
         * Begins recording of this command buffer.
         *
         * @param isSingleUse Specifies this command buffer will only be used once and will be reset and recorded again between each submission. Default: false
         * @param isRenderPassContinue Specifies that a secondary buffer is considered to be entirely inside a render pass. Default: false
         * @param isSimultaneousUse Specifies that the buffer can be resubmitted to a queue while it is in pending state and recorded into multiple primary buffers. Default: false
         */
        virtual void Begin( const bool isSingleUse = false, const bool isRenderPassContinue = false, const bool isSimultaneousUse = false ) = 0;

        /**
         * Ends recording of this command buffer. From here, the buffer may be submitted to a queue for execution.
         */
        virtual void End() = 0;
    };
}