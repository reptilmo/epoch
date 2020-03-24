#pragma once

#include "../../../Types.h"
#include <vulkan/vulkan.h>

namespace Epoch {

    class VulkanDevice;

    class VulkanInternalBuffer {
    public:
        VulkanInternalBuffer( VulkanDevice* device, U64 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags, const bool bindOnCreate = true );
        ~VulkanInternalBuffer();

        void Bind( U64 offset = 0 );

        void* LockMemory( U64 offset, U64 size, U64 flags );
        void UnlockMemory();

        void CopyTo( VulkanInternalBuffer* other, U64 sourceOffset, U64 destinationOffset, U64 size ) const;
        void CopyFrom( const VulkanInternalBuffer* other, U64 sourceOffset, U64 destinationOffset, U64 size );

        VkBuffer GetHandle() { return _handle; }
        VkDeviceMemory GetMemory() { return _memory; }
    private:
        VkBufferUsageFlags _usage;
        VulkanDevice* _device;
        bool _isLocked = false;
        VkBuffer _handle;
        VkDeviceMemory _memory;
    };
}