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

    struct VulkanBufferDataRange {
        U64 ElementCount;
        U64 ElementSize;
        U64 Offset;
    };

    template <class T>
    class VulkanBuffer {
    public:
        VulkanBuffer( VulkanRenderer* renderer, VulkanBufferType type ) {
            _renderer = renderer;
            _bufferType = type;
            //_elementCount = 0;
        }
        virtual ~VulkanBuffer() {
            destroy();
            _renderer = nullptr;
        }

        VkBuffer GetHandle() { return _buffer; }
        const std::vector<VulkanBufferDataRange> GetDataRanges() { return _dataRanges; }

        /**
         * Sets all data in the buffer.
         *
         * @data The data to be set.
         */
        virtual void SetData( std::vector<T> data );

        virtual void Allocate( U64 size );

        virtual void SetDataRange( std::vector<T> data, U64 offset );

        void* Map( U64 size );
        void Unmap();

    private:
        VkBufferUsageFlagBits getUsageFlag();
        void destroy();
    private:
        U64 _totalSize;
        std::vector<VulkanBufferDataRange> _dataRanges;
        VulkanRenderer* _renderer;
        VulkanBufferType _bufferType;
        VkBuffer _buffer;
        VkDeviceMemory _memory;
    };

    template <class T>
    void VulkanBuffer<T>::SetData( std::vector<T> data ) {
        ASSERT( data.size() > 0 );

        VulkanBufferDataRange dataRange;
        dataRange.ElementCount = (U32)data.size();
        dataRange.ElementSize = sizeof( data[0] );
        dataRange.Offset = 0;
        _dataRanges.push_back( dataRange );

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

        Allocate( bufferSize );

        //// Destroy if already exists.
        //destroy();

        //VulkanUtilities::CreateBuffer( physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | getUsageFlag(),
        //    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _buffer, _memory );

        // Perform the copy.
        VulkanUtilities::CopyBuffer( _renderer, stagingBuffer, _buffer, bufferSize, _renderer->GetCommandPool() );

        // Clean up the staging buffer and memory.
        vkDestroyBuffer( device, stagingBuffer, nullptr );
        vkFreeMemory( device, stagingBufferMemory, nullptr );
    }

    template <class T>
    void VulkanBuffer<T>::Allocate( U64 size ) {

        // Make sure to nuke the existing buffer first so it is no leaked.
        destroy();

        _totalSize = size;

        VkPhysicalDevice physicalDevice = _renderer->GetPhysicalDeviceHandle();
        VkDevice device = _renderer->GetDeviceHandle();

        // Setup a device-local buffer as the actual buffer. Data will be copied to this from the staging buffer. Mark it as
        // the destination of the transfer.
        VulkanUtilities::CreateBuffer( physicalDevice, device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | getUsageFlag(),
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _buffer, _memory );
    }

    template <class T>
    void VulkanBuffer<T>::SetDataRange( std::vector<T> data, U64 offset ) {

        ASSERT( data.size() > 0 );

        VulkanBufferDataRange dataRange;
        dataRange.ElementCount = (U32)data.size();
        dataRange.ElementSize = sizeof( data[0] );
        dataRange.Offset = offset;
        _dataRanges.push_back( dataRange );

        VkPhysicalDevice physicalDevice = _renderer->GetPhysicalDeviceHandle();
        VkDevice device = _renderer->GetDeviceHandle();

        VkDeviceSize dataSize = sizeof( data[0] ) * data.size();
        ASSERT_MSG( offset + dataSize <= _totalSize, "Buffer does not have enough room for data to be added." );

        // Create a host-visible staging buffer to upload to. Mark it as the source of the transfer.
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VulkanUtilities::CreateBuffer( physicalDevice, device, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

        // Obtain a pointer to the mapped memory by mapping it
        void* pData;
        VK_CHECK( vkMapMemory( device, stagingBufferMemory, 0, dataSize, 0, &pData ) );

        // Copy data to the memory
        memcpy( pData, data.data(), dataSize );

        // Unmap the memory.
        vkUnmapMemory( device, stagingBufferMemory );

        // Perform the copy from staging to the device local buffer.
        VulkanUtilities::CopyBufferOffset( _renderer, stagingBuffer, 0, _buffer, offset, dataSize, _renderer->GetCommandPool() );

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

    template <class T>
    void VulkanBuffer<T>::destroy() {
        if( _buffer ) {
            vkDestroyBuffer( _renderer->GetDeviceHandle(), _buffer, nullptr );
            _buffer = nullptr;
        }

        if( _memory ) {
            vkFreeMemory( _renderer->GetDeviceHandle(), _memory, nullptr );
            _memory = nullptr;
        }

        _dataRanges.clear();
    }
}