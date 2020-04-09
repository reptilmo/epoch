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
    };
}