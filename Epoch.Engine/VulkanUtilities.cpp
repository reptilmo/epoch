
#include "Logger.h"

#include "VulkanUtilities.h"

namespace Epoch {

    U32 VulkanUtilities::FindMemoryType( VkPhysicalDevice physicalDevice, U32 typeFilter, VkMemoryPropertyFlags properties ) {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties( physicalDevice, &memoryProperties );

        for( U32 i = 0; i < memoryProperties.memoryTypeCount; ++i ) {

            // Check each memory type to see if its bit is set to 1.
            if( typeFilter & ( 1 << i ) && ( memoryProperties.memoryTypes[i].propertyFlags & properties ) == properties ) {
                return i;
            }
        }

        Logger::Fatal( "Unable to find suitable memory type!" );
        return 255;
    }

    VkFormat VulkanUtilities::FindDepthFormat( VkPhysicalDevice physicalDevice ) {
        const U64 candidateCount = 3;
        VkFormat candidates[candidateCount] = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };
        return VulkanUtilities::FindSupportedFormat( physicalDevice, candidates, candidateCount, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
    }

    VkFormat VulkanUtilities::FindSupportedFormat( VkPhysicalDevice physicalDevice, const VkFormat* candidates, const U64 canditateCount, U32 tiling, U32 featureFlags ) {
        for( U64 i = 0; i < canditateCount; ++i ) {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties( physicalDevice, (VkFormat)candidates[i], &properties );

            if( tiling == VK_IMAGE_TILING_OPTIMAL && ( properties.linearTilingFeatures & featureFlags ) == featureFlags ) {
                return candidates[i];
            } else if( tiling == VK_IMAGE_TILING_OPTIMAL && ( properties.optimalTilingFeatures & featureFlags ) == featureFlags ) {
                return candidates[i];
            }
        }

        Logger::Fatal( "Failed to find a supported format!" );
        return VkFormat::VK_FORMAT_UNDEFINED;
    }
}