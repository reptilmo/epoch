#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "VulkanUtilities.h"
#include "VulkanRenderer.h"
#include "../../../Defines.h"

namespace Epoch {

    enum class VulkanBufferType {
        VERTEX,
        INDEX,
        UNIFORM
    };

    template <class T>
    class VulkanBuffer {
    public:
        VulkanBuffer( VulkanRenderer* renderer, VulkanBufferType type ) {
            _renderer = renderer;
            _bufferType = type;
            _elementCount = 0;
        }
        virtual ~VulkanBuffer() {
            if( _buffer ) {
                vkDestroyBuffer( _renderer->GetDeviceHandle(), _buffer, nullptr );
                _buffer = nullptr;
            }

            if( _memory ) {
                vkFreeMemory( _renderer->GetDeviceHandle(), _memory, nullptr );
                _memory = nullptr;
            }
            _renderer = nullptr;
        }

        VkBuffer GetHandle() { return _buffer; }
        U32 GetElementCount() { return _elementCount; }

        virtual void SetData( std::vector<T> data );

        void* Map( U64 size );
        void Unmap();

    private:
        VkBufferUsageFlagBits getUsageFlag();
        U32 _elementCount;
    private:
        VulkanRenderer* _renderer;
        VulkanBufferType _bufferType;
        VkBuffer _buffer;
        VkDeviceMemory _memory;
    };

    template <class T>
    void VulkanBuffer<T>::SetData( std::vector<T> data ) {
        ASSERT( data.size() > 0 );

        _elementCount = (U32)data.size();

        VkPhysicalDevice physicalDevice = _renderer->GetPhysicalDeviceHandle();
        VkDevice device = _renderer->GetDeviceHandle();

        VkDeviceSize bufferSize = sizeof( data[0] ) * data.size();

        // Create a host-visible staging buffer to upload to. Mark it as the source of the transfer.
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VulkanUtilities::CreateBuffer( physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

        // Obtain a pointer to the mapped memory by mapping it
        void* pData;
        VK_CHECK( vkMapMemory( device, stagingBufferMemory, 0, bufferSize, 0, &pData ) ); // NOTE: Assumes 0 offset.

        // Copy data to the memory
        memcpy( pData, data.data(), bufferSize );

        // Unmap the memory.
        vkUnmapMemory( device, stagingBufferMemory );

        // Setup a device-local buffer as the actual buffer. Data will be copied to this from the staging buffer. Mark it as
        // the destination of the transfer.
        if( _buffer ) {
            // Destroy if already exists.
            vkDestroyBuffer( device, _buffer, nullptr );
            vkFreeMemory( device, _memory, nullptr );
        }

        VulkanUtilities::CreateBuffer( physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | getUsageFlag(),
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _buffer, _memory );

        // Perform the copy.
        VulkanUtilities::CopyBuffer( _renderer, stagingBuffer, _buffer, bufferSize, _renderer->GetCommandPool() );

        // Clean up the staging buffer and memory.
        vkDestroyBuffer( device, stagingBuffer, nullptr );
        vkFreeMemory( device, stagingBufferMemory, nullptr );
    }

    template<class T>
    void* VulkanBuffer<T>::Map( U64 size ) {
        void* data;
        VK_CHECK( vkMapMemory( _renderer->GetDeviceHandle(), _memory, 0, size, 0, &data ) );
        return data;
    }

    template<class T>
    void VulkanBuffer<T>::Unmap() {
        vkUnmapMemory( _renderer->GetDeviceHandle(), _memory );
    }

    template <class T>
    VkBufferUsageFlagBits VulkanBuffer<T>::getUsageFlag() {
        switch( _bufferType ) {
        case VulkanBufferType::VERTEX:
            return VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        case VulkanBufferType::INDEX:
            return VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        case VulkanBufferType::UNIFORM:
            return VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
    }
}