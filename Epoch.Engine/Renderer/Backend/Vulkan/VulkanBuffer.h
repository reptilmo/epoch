#pragma once

#include <vector>
#include <map>
#include <vulkan/vulkan.h>

#include "VulkanUtilities.h"
#include "VulkanDevice.h"
#include "VulkanInternalBuffer.h"
#include "../../../Logger.h"
#include "../../../Defines.h"
#include "../../../Memory/Memory.h"
#include "../../../Containers/LinkedList.h"

namespace Epoch {

    enum class VulkanBufferType {
        VERTEX,
        INDEX,
        UNIFORM
    };

    struct VulkanBufferDataBlock {
        U64 ElementCount;
        U64 ElementSize;
        U64 Offset;
        U64 HeapIndex;
        U64 BlockSize;
        bool Allocated = false;

        void CopyFrom( const VulkanBufferDataBlock& other ) {
            ElementCount = other.ElementCount;
            ElementSize = other.ElementSize;
            Offset = other.Offset;
            HeapIndex = other.HeapIndex;
            BlockSize = other.BlockSize;
            Allocated = other.Allocated;
        }
    };

    /**
     * Represents a base-level Vulkan-specific buffer to be used for various purposes.
     * Internally tracks allocations/deallocations and keeps track of offsets.
     */
    template <class T>
    class VulkanBuffer {
    public:

        /**
         * Creates a new Vulkan Buffer
         *
         * @param device The VulkanDevice associated with this buffer.
         * @param type The Vulkan buffer type of this buffer.
         */
        VulkanBuffer( VulkanDevice* device, VulkanBufferType type ) {
            _device = device;
            _bufferType = type;
        }

        /**
         * Destroys this buffer.
         */
        virtual ~VulkanBuffer() {
            destroy();
            _device = nullptr;
        }

        /**
         * Returns the internal buffer handle.
         */
        VkBuffer GetHandle() { return _internalBuffer->GetHandle(); }
        
        /**
         * Returns the internal buffer.
         */
        VulkanInternalBuffer* GetInternal() { return _internalBuffer; }

        /**
         * Returns all data ranges in this buffer. TODO: This should be removed in favour of retrieving
         * by ObjectID only.
         */
        const LinkedListNode<VulkanBufferDataBlock>* PeekDataBlock() { return _allocations.Peek(); }

        /**
         * Sets all data in the buffer. Calls Allocate() first.
         *
         * @data The data to be set.
         */
        virtual void SetData( std::vector<T> data );

        /**
         * Allocates internal buffers. This is destructive to any that may already exist, which are
         * destroyed before a new internal buffer is created.
         *
         * The size of the buffer in bytes to create.
         */
        virtual void Allocate( U64 size );

        /**
         * Sets data in the buffer starting at the offset.
         *
         * @param data The data to be set.
         * @param offset The offset in bytes to set the data.
         */
        virtual void SetDataRange( std::vector<T> data, U64 offset );

        /**
         * Frees data in the buffer starting at the offset.
         *
         * @param data The data to be set.
         * @param offset The offset in bytes to set the data.
         */
        virtual void FreeDataRange( const VulkanBufferDataBlock& range );

        /**
         * Frees the specified amount of data in the buffer starting at the offset.
         *
         * @param offset The offset in bytes to set the data.
         * @param size The total size in bytes of the data to be freed.
         */
        virtual void FreeDataRange( const U64 offset, const U64 size );

    private:
        VkBufferUsageFlagBits getUsageFlag();
        void destroy();
        U64 getObjectId();
    private:
        U64 _totalSize;

        // The heap index, used to keep a unique index per allocation.
        U64 _heapIndex = 0;
        VulkanDevice* _device;
        VulkanBufferType _bufferType;
        VulkanInternalBuffer* _internalBuffer;

        // A listing of allocations kept within this buffer. Used to maintain block sizes and offsets.
        LinkedList<VulkanBufferDataBlock> _allocations;
    };

    template <class T>
    void VulkanBuffer<T>::SetData( std::vector<T> data ) {
        ASSERT( data.size() > 0 );

        VkDeviceSize bufferSize = sizeof( data[0] ) * data.size();

        // Create a host-visible staging buffer to upload to. Mark it as the source of the transfer.
        VkBufferUsageFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        VulkanInternalBuffer staging( _device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, flags );

        // Lock, copy, unlock
        void* pData = staging.LockMemory( 0, bufferSize, 0 );
        TMemory::Memcpy( pData, data.data(), bufferSize );
        staging.UnlockMemory();

        // Setup a device-local buffer as the actual buffer. Data will be copied to this from the staging buffer. Mark it as
        // the destination of the transfer.
        Allocate( bufferSize );

        // Mark the head's object id
        LinkedListNode<VulkanBufferDataBlock>* head = _allocations.Peek();
        head->Value.HeapIndex = getObjectId();
        head->Value.Allocated = true;

        // Perform the copy.
        staging.CopyTo( _internalBuffer, 0, 0, bufferSize );
    }

    template <class T>
    void VulkanBuffer<T>::Allocate( U64 size ) {

        // Make sure to nuke the existing buffer first so it is no leaked.
        destroy();

        _totalSize = size;

        // One data block for the entire space.
        VulkanBufferDataBlock entireBlock;
        entireBlock.ElementSize = size;
        entireBlock.ElementCount = 1;
        entireBlock.BlockSize = entireBlock.ElementSize * entireBlock.ElementCount;
        entireBlock.Allocated = false;
        entireBlock.Offset = 0;
        entireBlock.HeapIndex = -1;
        _allocations.Push( entireBlock );

        // Setup a device-local buffer as the actual buffer. Data will be copied to this from the staging buffer. Mark it as
        // the destination of the transfer.
        VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | getUsageFlag();
        VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        _internalBuffer = new VulkanInternalBuffer( _device, size, usage, flags, true );
    }

    template <class T>
    void VulkanBuffer<T>::SetDataRange( std::vector<T> data, U64 offset ) {

        ASSERT( data.size() > 0 );

        VulkanBufferDataBlock dataBlock;
        dataBlock.ElementCount = (U32)data.size();
        dataBlock.ElementSize = sizeof( data[0] );
        dataBlock.BlockSize = dataBlock.ElementSize * dataBlock.ElementCount;
        dataBlock.Allocated = true;
        dataBlock.Offset = offset;
        dataBlock.HeapIndex = getObjectId();

        // Update tracking info.
        bool blockFound = false;
        U64 index = 0;
        U64 runningOffset = 0;
        while( !blockFound ) {
            LinkedListNode<VulkanBufferDataBlock>* p = _allocations.Peek();
            while( p->Value.Allocated ) {
                runningOffset += p->Value.Offset;
                p = p->Next;
                ++index;
            }

            // If it will fit in this block, use it. Otherwise, move on.
            if( p->Value.BlockSize >= dataBlock.BlockSize ) {
                Logger::Trace( "DestBlock: %dB, SrcBlock: %dB. Fits, DestBlock new Offset: %dB", p->Value.BlockSize, dataBlock.BlockSize, runningOffset );

                // See if the data will fit in this block.
                U64 diff = p->Value.BlockSize - dataBlock.BlockSize;

                // Block is an excact fit. Do a straight swap-out
                if( diff == 0 ) {
                    p->Value.CopyFrom( dataBlock );
                    p->Value.Allocated = true;
                    p->Value.Offset = runningOffset;
                    blockFound = true;
                } else {
                    // Block must be larger than needed, so it will fit.

                    // First adjust block to contain the right offset and amount of memory.
                    p->Value.BlockSize -= dataBlock.BlockSize;
                    p->Value.ElementSize -= dataBlock.BlockSize;
                    p->Value.Offset += dataBlock.BlockSize;

                    // Insert.
                    _allocations.InsertAt( dataBlock, index );
                    blockFound = true;
                }
            } else {
                Logger::Trace( "DestBlock: %dB, SrcBlock: %dB. DestBlock too small. Continuing...", p->Value.BlockSize, dataBlock.BlockSize );

                // Block too small. Keep looking.
                ++index;
                runningOffset += p->Value.Offset;
            }
        }

        VkDeviceSize dataSize = sizeof( data[0] ) * data.size();
        ASSERT_MSG( offset + dataSize <= _totalSize, "Buffer does not have enough room for data to be added." );

        // Create a host-visible staging buffer to upload to. Mark it as the source of the transfer.
        VkBufferUsageFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        VulkanInternalBuffer staging( _device, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, flags );

        // Lock, copy, unlock
        void* pData = staging.LockMemory( 0, dataSize, 0 );
        TMemory::Memcpy( pData, data.data(), dataSize );
        staging.UnlockMemory();

        // Perform the copy from staging to the device local buffer.
        staging.CopyTo( _internalBuffer, 0, offset, dataSize );
    }

    template<class T>
    void VulkanBuffer<T>::FreeDataRange( const VulkanBufferDataBlock& range ) {
        U64 size = range.BlockSize;
        FreeDataRange( range.Offset, size );
    }

    template<class T>
    void VulkanBuffer<T>::FreeDataRange( const U64 offset, const U64 size ) {

        // Don't actually bother "resetting" the data. Just mark the range of memory as available and
        // remove the range from the map.

        LinkedListNode<VulkanBufferDataBlock>* p = _allocations.Peek();
        LinkedListNode<VulkanBufferDataBlock>* prev = nullptr;
        U64 index = 0;
        while( p->Value.Offset != offset ) {
            if( p->Next != nullptr ) {
                prev = p;
                p = p->Next;
                ++index;
            }
        }

        // Double-check that the block found is at the right offset. If not, something has gone wrong,
        // Typically the caller tried to free something at an offset that doesn't make sense.
        if( p->Value.Offset == offset ) {
            p->Value.Allocated = false;
            p->Value.BlockSize = size;
            p->Value.ElementCount = 1;
            p->Value.ElementSize = size;
            p->Value.HeapIndex = 0;
        } else {
            Logger::Fatal( "Attempted to call FreeDataRange with an invalid offset of %d, which does not match any allocation present on this buffer.", offset );
        }

        // Previous block is unallocated. Combine the two by increasing 
        // the size of that one and removing this one
        if( prev != nullptr && !prev->Value.Allocated ) {
            prev->Value.BlockSize += p->Value.BlockSize;
            prev->Value.ElementSize += p->Value.ElementSize;
            _allocations.RemoveAt( index );
        }

        // Check if the next block is unallocated. If so, combine it with
        // this one and remove it.
        if( p->Next != nullptr && !p->Next->Value.Allocated ) {
            p->Value.BlockSize += p->Next->Value.BlockSize;
            p->Value.ElementSize += p->Next->Value.ElementSize;
            _allocations.RemoveAt( index + 1 );
        }
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
        default:
            Logger::Fatal( "Invalid usage. This should never happen." );
            return VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
    }

    template <class T>
    void VulkanBuffer<T>::destroy() {
        if( _internalBuffer ) {
            delete _internalBuffer;
            _internalBuffer = nullptr;
        }
    }

    template <class T>
    U64 VulkanBuffer<T>::getObjectId() {
        return _heapIndex++;
    }
}