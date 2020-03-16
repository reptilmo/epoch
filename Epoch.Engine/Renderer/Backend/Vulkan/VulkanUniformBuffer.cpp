
#include "VulkanUniformBuffer.h"

namespace Epoch {

    VulkanUniformBuffer::VulkanUniformBuffer( VulkanRenderer* renderer ) : VulkanBuffer( renderer, VulkanBufferType::UNIFORM ) {

    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {
        VulkanBuffer::~VulkanBuffer();
    }

    void VulkanUniformBuffer::SetData( std::vector<Epoch::StandardUniformBufferObject> data ) {
        VulkanBuffer::SetData( data );
    }
}