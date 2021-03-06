#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "../../../Types.h"

namespace Epoch {

    class VulkanDevice;
    class VulkanCommandBuffer;

    class VulkanCommandPool {
    public:
        VulkanCommandPool( VulkanDevice* device, const U32 queueFamilyIndex, const bool buffersCanReset = true, const bool buffersProtected = false, const bool transient = false );
        ~VulkanCommandPool();

        VulkanCommandBuffer* AllocateCommandBuffer( const bool isPrimary );
        void FreeCommandBuffer( VulkanCommandBuffer* commandBuffer );

        const U32 GetQueueFamilyIndex() const { return _queueFamilyIndex; }
        VkCommandPool GetHandle() { return _handle; }
    private:
        U32 _queueFamilyIndex;
        VulkanDevice* _device;
        VkCommandPool _handle;
        std::vector<VulkanCommandBuffer*> _inUseBuffers;
        std::vector<VulkanCommandBuffer*> _availableBuffers;
    };
}