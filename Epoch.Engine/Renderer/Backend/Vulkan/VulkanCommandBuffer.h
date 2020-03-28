#pragma once

#include <vulkan/vulkan.h>

namespace Epoch {

    class VulkanDevice;

    class VulkanCommandBuffer {
    public:
        VulkanCommandBuffer( VulkanDevice* device );
        ~VulkanCommandBuffer();



        VkCommandBuffer GetHandle() { return _handle; }

    private:
        VkCommandBuffer _handle;
    };
}