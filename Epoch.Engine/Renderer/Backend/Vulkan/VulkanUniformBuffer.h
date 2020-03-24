#pragma once


#pragma once

#include "VulkanBuffer.h"
#include "../../StandardUniformBufferObject.h"

namespace Epoch {

    class VulkanDevice;

    class VulkanUniformBuffer : public VulkanBuffer<Epoch::StandardUniformBufferObject> {
    public:
        VulkanUniformBuffer( VulkanDevice* device );
        ~VulkanUniformBuffer();

        void SetData( std::vector<Epoch::StandardUniformBufferObject> data );
    };
}