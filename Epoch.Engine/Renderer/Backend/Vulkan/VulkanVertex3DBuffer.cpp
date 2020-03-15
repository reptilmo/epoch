
#include "VulkanVertex3DBuffer.h"

namespace Epoch {

    VulkanVertex3DBuffer::VulkanVertex3DBuffer( VulkanRenderer* renderer ) : VulkanBuffer( renderer, VulkanBufferType::VERTEX ) {

    }

    VulkanVertex3DBuffer::~VulkanVertex3DBuffer() {
        VulkanBuffer::~VulkanBuffer();
    }

    void VulkanVertex3DBuffer::SetData( std::vector<Vertex3D> data ) {
        VulkanBuffer::SetData( data );
    }
}