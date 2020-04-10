
#include <vulkan/vulkan.h>

#include "VulkanCommandBuffer.h"
#include "VulkanVertex3DBuffer.h"

namespace Epoch {

    VulkanVertex3DBuffer::VulkanVertex3DBuffer( VulkanDevice* device ) : VulkanBuffer( device, VulkanBufferType::VERTEX ) {

    }

    VulkanVertex3DBuffer::~VulkanVertex3DBuffer() {
        VulkanBuffer::~VulkanBuffer();
    }

    void VulkanVertex3DBuffer::SetData( std::vector<Vertex3D> data ) {
        VulkanBuffer::SetData( data );
    }

    void VulkanVertex3DBuffer::Bind( ICommandBuffer* commandBuffer, const U64 offset ) {
        const VkBuffer handle = GetHandle();
        vkCmdBindVertexBuffers( static_cast<VulkanCommandBuffer*>( commandBuffer )->GetHandle(), 0, 1, &handle, static_cast<const VkDeviceSize*>( &offset ) );
    }
}