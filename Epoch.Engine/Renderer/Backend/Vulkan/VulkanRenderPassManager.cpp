
#include "VulkanDevice.h"
#include "VulkanUtilities.h"
#include "VulkanRenderPassManager.h"

namespace Epoch {

    VkRenderPass _tempRenderPass = nullptr;

    const bool VulkanRenderPassManager::CreateRenderPass( VulkanDevice* device, const VkRenderPassCreateInfo& info ) {

        VK_CHECK( vkCreateRenderPass( device->LogicalDevice, &info, nullptr, &_tempRenderPass ) );
        return true;
    }

    VkRenderPass VulkanRenderPassManager::GetRenderPass( const char* renderPassName ) {
        return _tempRenderPass;
    }

    void VulkanRenderPassManager::DestroyRenderPass( VulkanDevice* device, const char* renderPassName ) {
        vkDestroyRenderPass( device->LogicalDevice, _tempRenderPass, nullptr );
    }
}