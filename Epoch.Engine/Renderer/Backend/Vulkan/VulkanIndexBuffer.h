#pragma once

#include "VulkanBuffer.h"

namespace Epoch {

    class VulkanDevice;

    class VulkanIndexBuffer : public VulkanBuffer<U32> {
    public:
        VulkanIndexBuffer( VulkanDevice* device );
        ~VulkanIndexBuffer();

        void SetData( std::vector<U32> data );
    };
}