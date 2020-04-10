#pragma once


#pragma once

#include "VulkanBuffer.h"
#include "../../StandardUniformBufferObject.h"
#include "../../IUniformBuffer.h"

namespace Epoch {

    class VulkanDevice;

    class VulkanUniformBuffer : public VulkanBuffer<Epoch::StandardUniformBufferObject>, public IUniformBuffer {
    public:
        VulkanUniformBuffer( VulkanDevice* device );
        ~VulkanUniformBuffer();

        void SetData( std::vector<Epoch::StandardUniformBufferObject> data );

        /**
         * Binds this buffer using the given offset.
         *
         * @param offset The offset in bytes to be bound.
         */
        virtual void Bind( ICommandBuffer* commandBuffer, const U64 offset ) override {
        }
    };
}