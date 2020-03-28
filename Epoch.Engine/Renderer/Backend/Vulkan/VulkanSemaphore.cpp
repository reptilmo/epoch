
#include "VulkanDevice.h"
#include "VulkanUtilities.h"
#include "VulkanSemaphore.h"

namespace Epoch {

    VulkanSemaphore::VulkanSemaphore( VulkanDevice* device ) {
        _device = device;

        VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };        
        VK_CHECK( vkCreateSemaphore( _device->LogicalDevice, &semaphoreCreateInfo, nullptr, &_handle ) );
    }

    VulkanSemaphore::~VulkanSemaphore() {
        if( _handle ) {
            vkDestroySemaphore( _device->LogicalDevice, _handle, nullptr );
            _handle = nullptr;
        }
    }
}