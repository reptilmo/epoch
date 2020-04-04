#pragma once

#include <vulkan/vulkan.h>

#include "../../../Types.h"

namespace Epoch {

    class VulkanDevice;

    class VulkanTextureSampler {
    public:
        VulkanTextureSampler( VulkanDevice* device );
        ~VulkanTextureSampler();

        VkSampler GetHandle() { return _handle; }

    private:
        VulkanDevice* _device;
        VkSampler _handle;
    };
}