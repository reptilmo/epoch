#pragma once

#include "VulkanBuffer.h"

namespace Epoch {

    class VulkanRenderer;

    class VulkanIndexBuffer : public VulkanBuffer<U32> {
    public:
        VulkanIndexBuffer( VulkanRenderer* renderer );
        ~VulkanIndexBuffer();

        void SetData( std::vector<U32> data );
    };
}