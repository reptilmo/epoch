
#include "VulkanDevice.h"
#include "VulkanUtilities.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"

namespace Epoch {

    VulkanCommandPool::VulkanCommandPool( VulkanDevice* device, const U32 queueFamilyIndex, const bool buffersCanReset, const bool buffersProtected, const bool transient ) {
        _device = device;
        _queueFamilyIndex = queueFamilyIndex;

        VkCommandPoolCreateInfo poolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        poolCreateInfo.queueFamilyIndex = _queueFamilyIndex;
        poolCreateInfo.flags = 0;
        if( buffersCanReset ) {
            poolCreateInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        }
        if( buffersProtected ) {
            poolCreateInfo.flags |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
        }
        if( transient ) {
            poolCreateInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        }
        VK_CHECK( vkCreateCommandPool( _device->LogicalDevice, &poolCreateInfo, nullptr, &_handle ) );
    }

    VulkanCommandPool::~VulkanCommandPool() {
        if( _handle ) {
            vkDestroyCommandPool( _device->LogicalDevice, _handle, nullptr );
            _handle = nullptr;
        }

        _queueFamilyIndex = -1;
        _device = nullptr;
    }

    VulkanCommandBuffer* VulkanCommandPool::AllocateCommandBuffer( const bool isPrimary ) {
        if( _availableBuffers.size() == 0 ) {
            VkCommandBuffer handle;
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
            commandBufferAllocateInfo.commandPool = _handle;
            commandBufferAllocateInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
            commandBufferAllocateInfo.commandBufferCount = 1;

            VK_CHECK( vkAllocateCommandBuffers( _device->LogicalDevice, &commandBufferAllocateInfo, &handle ) );
            return new VulkanCommandBuffer( handle, this, isPrimary );
        } else {
            // TODO: recycle these objects.
        }

    }

    void VulkanCommandPool::FreeCommandBuffer( VulkanCommandBuffer* commandBuffer ) {
        VkCommandBuffer handle = commandBuffer->GetHandle();
        vkFreeCommandBuffers( _device->LogicalDevice, _handle, 1, &handle );
        
        // TODO: recycle these objects.
        delete commandBuffer;
        commandBuffer = nullptr;
    }
}