
#include "../../../Logger.h"

#include "VulkanDevice.h"
#include "VulkanUtilities.h"
#include "VulkanFence.h"

namespace Epoch {

    VulkanFence::VulkanFence( VulkanDevice* device, const bool createSignaled ) {
        _device = device;
        _state = VulkanFenceState::NotReady;

        VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        if( createSignaled ) {
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            _state = VulkanFenceState::Signaled;
        }
        VK_CHECK( vkCreateFence( _device->LogicalDevice, &fenceCreateInfo, nullptr, &_handle ) );
    }

    VulkanFence::~VulkanFence() {
        if( _handle ) {
            vkDestroyFence( _device->LogicalDevice, _handle, nullptr );
            _handle = nullptr;
        }
        _device = nullptr;
        _state = VulkanFenceState::NotReady;
    }

    const bool VulkanFence::Wait( U64 timeoutNS ) {
        if( _state == VulkanFenceState::NotReady ) {
            VkResult result = vkWaitForFences( _device->LogicalDevice, 1, &_handle, true, timeoutNS );
            switch( result )
            {
            case VK_SUCCESS:
                _state = VulkanFenceState::Signaled;
                return true;
            case VK_TIMEOUT:
                Logger::Warn( "VulkanFence::Wait() - Timed out" );
                break;
            case VK_ERROR_DEVICE_LOST:
                Logger::Error( "VulkanFence::Wait() - VK_ERROR_DEVICE_LOST." );
                break;
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                Logger::Error( "VulkanFence::Wait() - VK_ERROR_OUT_OF_HOST_MEMORY." );
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                Logger::Error( "VulkanFence::Wait() - VK_ERROR_OUT_OF_DEVICE_MEMORY." );
                break;
            default:
                Logger::Error( "VulkanFence::Wait() - An unknown error has occurred." );
                break;
            }
        } else {

            // If already signaled, no wait is needed.
            return true;
        }

        return false;
    }

    void VulkanFence::Reset() {
        if( _state != VulkanFenceState::NotReady ) {
            VK_CHECK( vkResetFences( _device->LogicalDevice, 1, &_handle ) );
            _state = VulkanFenceState::NotReady;
        }
    }
}