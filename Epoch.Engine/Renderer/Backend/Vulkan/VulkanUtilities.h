#pragma once

#include <vulkan/vulkan.h>
#include "../../../Defines.h"
#include "../../../Types.h"

#define VK_CHECK(expr) { \
    ASSERT(expr == VK_SUCCESS); \
}

namespace Epoch {

    class VulkanRenderer;

    class VulkanUtilities {
    public:
        static U32 FindMemoryType( VkPhysicalDevice physicalDevice, U32 typeFilter, VkMemoryPropertyFlags properties );
        static VkFormat FindDepthFormat( VkPhysicalDevice physicalDevice );
        static VkFormat FindSupportedFormat( VkPhysicalDevice physicalDevice, const VkFormat* candidates, const U64 canditateCount, U32 tiling, U32 featureFlags );

        static void CreateBuffer( VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize deviceSize, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer& buffer, VkDeviceMemory& memory );
        static void CopyBuffer( VulkanRenderer* renderer, VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandPool commandPool );
        static void CopyBufferOffset( VulkanRenderer* renderer, VkBuffer source, U64 sourceOffset, VkBuffer destination, U64 destinationOffset, VkDeviceSize size, VkCommandPool commandPool );
    };
}