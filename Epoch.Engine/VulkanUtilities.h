#pragma once

#include <vulkan/vulkan.h>
#include "Defines.h"
#include "Types.h"

#define VK_CHECK(expr) { \
    ASSERT(expr == VK_SUCCESS); \
}

namespace Epoch {

    class VulkanUtilities {
    public:
        static U32 FindMemoryType( VkPhysicalDevice physicalDevice, U32 typeFilter, VkMemoryPropertyFlags properties );
        static VkFormat FindDepthFormat( VkPhysicalDevice physicalDevice );
        static VkFormat FindSupportedFormat( VkPhysicalDevice physicalDevice, const VkFormat* candidates, const U64 canditateCount, U32 tiling, U32 featureFlags );
    };
}