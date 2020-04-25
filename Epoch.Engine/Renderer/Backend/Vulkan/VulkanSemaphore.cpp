
#include "VulkanDevice.h"
#include "VulkanUtilities.h"
#include "VulkanSemaphore.h"

namespace Epoch {

    VulkanSemaphore::VulkanSemaphore( VulkanDevice* device, const bool isSignaled ) {
        _device = device;
        IsSignaled = isSignaled;

        VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };        
        VK_CHECK( vkCreateSemaphore( _device->LogicalDevice, &semaphoreCreateInfo, nullptr, &Handle ) );
    }

    VulkanSemaphore::~VulkanSemaphore() {
        if( Handle ) {
            vkDestroySemaphore( _device->LogicalDevice, Handle, nullptr );
            Handle = nullptr;
        }
    }
}