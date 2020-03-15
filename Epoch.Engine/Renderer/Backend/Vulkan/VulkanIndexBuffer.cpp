
#include "VulkanIndexBuffer.h"

namespace Epoch {

    VulkanIndexBuffer::VulkanIndexBuffer( VulkanRenderer* renderer ) : VulkanBuffer( renderer, VulkanBufferType::INDEX ) {

    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {
        VulkanBuffer::~VulkanBuffer();
    }

    void VulkanIndexBuffer::SetData( std::vector<U32> data ) {
        VulkanBuffer::SetData( data );
    }
}