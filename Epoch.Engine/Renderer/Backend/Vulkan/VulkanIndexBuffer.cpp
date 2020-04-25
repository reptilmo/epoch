
#include "VulkanCommandBuffer.h"
#include "VulkanIndexBuffer.h"

namespace Epoch {

    VulkanIndexBuffer::VulkanIndexBuffer( VulkanDevice* device ) : VulkanBuffer( device, VulkanBufferType::INDEX ) {

    }

    VulkanIndexBuffer::~VulkanIndexBuffer() {
        VulkanBuffer::~VulkanBuffer();
    }

    void VulkanIndexBuffer::Bind( ICommandBuffer* commandBuffer, const U64 offset ) {
        vkCmdBindIndexBuffer( static_cast<VulkanCommandBuffer*>( commandBuffer )->Handle, GetHandle(), offset, VK_INDEX_TYPE_UINT32 );
    }
}