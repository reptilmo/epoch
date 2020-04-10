#pragma once

#include "VulkanBuffer.h"

namespace Epoch {

    class ICommandBuffer;
    class VulkanDevice;

    class VulkanIndexBuffer : public VulkanBuffer<U32> {
    public:
        VulkanIndexBuffer( VulkanDevice* device );
        ~VulkanIndexBuffer();

        void SetData( std::vector<U32> data );

        /**
         * Binds this buffer using the given offset.
         *
         * @param offset The offset in bytes to be bound.
         */
        virtual void Bind( ICommandBuffer* commandBuffer, const U64 offset ) override;
    };
}