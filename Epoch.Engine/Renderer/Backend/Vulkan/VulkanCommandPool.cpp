
#include "VulkanDevice.h"
#include "VulkanUtilities.h"
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
}