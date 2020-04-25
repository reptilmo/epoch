
#include "VulkanUtilities.h"
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanInternalBuffer.h"

namespace Epoch {


    VulkanInternalBuffer::VulkanInternalBuffer( VulkanDevice* device, U64 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags, const bool bindOnCreate ) {

        _device = device;
        _usage = usage;

        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE; // Only used in one queue.

        VK_CHECK( vkCreateBuffer( _device->LogicalDevice, &bufferInfo, nullptr, &_handle ) );

        // Gather memory requirements
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements( _device->LogicalDevice, _handle, &requirements );

        // Allocate memory info
        VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = requirements.size;

        allocInfo.memoryTypeIndex = VulkanUtilities::FindMemoryType( _device->PhysicalDevice, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

        // Allocate the memory and bind it
        VK_CHECK( vkAllocateMemory( _device->LogicalDevice, &allocInfo, nullptr, &_memory ) );

        // BindOnCreate assumes 0 offset.
        if( bindOnCreate ) {
            Bind( 0 );
        }
    }

    VulkanInternalBuffer::~VulkanInternalBuffer() {
        if( _device ) {
            if( _handle ) {
                vkDestroyBuffer( _device->LogicalDevice, _handle, nullptr );
                _handle = nullptr;
            }
            if( _memory ) {
                vkFreeMemory( _device->LogicalDevice, _memory, nullptr );
                _memory = nullptr;
            }

            _device = nullptr;
        }
    }

    void VulkanInternalBuffer::Bind( U64 offset ) {
        VK_CHECK( vkBindBufferMemory( _device->LogicalDevice, _handle, _memory, offset ) );
    }

    void* VulkanInternalBuffer::LockMemory( U64 offset, U64 size, U64 flags ) {
        void* pData;
        VK_CHECK( vkMapMemory( _device->LogicalDevice, _memory, offset, size, (U32)flags, &pData ) );
        return pData;
    }

    void VulkanInternalBuffer::UnlockMemory() {
        vkUnmapMemory( _device->LogicalDevice, _memory );
    }

    void VulkanInternalBuffer::CopyTo( VulkanInternalBuffer* other, U64 sourceOffset, U64 destinationOffset, U64 size ) const {

        ASSERT_MSG( _device == other->_device, "Buffers must be attached to the same device." );

        // Create a one-time use command buffer
        VulkanCommandBuffer* commandBuffer = _device->AllocateAndBeginSingleUseCommandBuffer();

        // Write the copy command.
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = sourceOffset;
        copyRegion.dstOffset = destinationOffset;
        copyRegion.size = size;
        vkCmdCopyBuffer( commandBuffer->Handle, _handle, other->_handle, 1, &copyRegion );

        _device->EndSingleUseCommandBuffer( commandBuffer );
    }

    void VulkanInternalBuffer::CopyFrom( const VulkanInternalBuffer* other, U64 sourceOffset, U64 destinationOffset, U64 size ) {
        ASSERT_MSG( _device == other->_device, "Buffers must be attached to the same device." );

        // Create a one-time use command buffer
        VulkanCommandBuffer* commandBuffer = _device->AllocateAndBeginSingleUseCommandBuffer();

        // Write the copy command.
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = sourceOffset;
        copyRegion.dstOffset = destinationOffset;
        copyRegion.size = size;
        vkCmdCopyBuffer( commandBuffer->Handle, other->_handle, _handle, 1, &copyRegion );

        _device->EndSingleUseCommandBuffer( commandBuffer );
    }
}