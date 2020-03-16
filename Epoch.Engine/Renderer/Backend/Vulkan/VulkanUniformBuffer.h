#pragma once


#pragma once

#include "VulkanBuffer.h"
#include "../../StandardUniformBufferObject.h"

namespace Epoch {

    class VulkanRenderer;

    class VulkanUniformBuffer : public VulkanBuffer<Epoch::StandardUniformBufferObject> {
    public:
        VulkanUniformBuffer( VulkanRenderer* renderer );
        ~VulkanUniformBuffer();

        void SetData( std::vector<Epoch::StandardUniformBufferObject> data );
    };
}