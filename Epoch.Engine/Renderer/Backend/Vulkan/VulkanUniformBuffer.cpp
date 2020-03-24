
#include "VulkanUniformBuffer.h"

namespace Epoch {

    VulkanUniformBuffer::VulkanUniformBuffer( VulkanDevice* device ) : VulkanBuffer( device, VulkanBufferType::UNIFORM ) {

    }

    VulkanUniformBuffer::~VulkanUniformBuffer() {
        VulkanBuffer::~VulkanBuffer();
    }

    void VulkanUniformBuffer::SetData( std::vector<Epoch::StandardUniformBufferObject> data ) {
        VulkanBuffer::SetData( data );
    }
}