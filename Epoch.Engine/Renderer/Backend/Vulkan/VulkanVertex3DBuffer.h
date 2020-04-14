#pragma once

#include "VulkanBuffer.h"
#include "../../Vertex3D.h"

namespace Epoch {

    class ICommandBuffer;
    class VulkanDevice;

    class VulkanVertex3DBuffer : public VulkanBuffer<Vertex3D> {
    public:
        VulkanVertex3DBuffer( VulkanDevice* device );
        ~VulkanVertex3DBuffer();

        void SetData( std::vector<Vertex3D> data );

        /**
         * Binds this buffer using the given offset.
         *
         * @param offset The offset in bytes to be bound.
         */
        virtual void Bind( ICommandBuffer* commandBuffer, const U64 offset ) override;
    };
}