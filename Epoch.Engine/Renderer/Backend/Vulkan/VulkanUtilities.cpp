
#include "../../../Logger.h"

#include "VulkanRenderer.h"
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

    void VulkanUtilities::CreateBuffer( VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize deviceSize,
        VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkBuffer& buffer, VkDeviceMemory& memory ) {

        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = deviceSize;
        bufferInfo.usage = usageFlags;
        bufferInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE; // Only used in one queue.

        VK_CHECK( vkCreateBuffer( device, &bufferInfo, nullptr, &buffer ) );

        // Gather memory requirements
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements( device, buffer, &requirements );

        // Allocate memory info
        VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = requirements.size;

        allocInfo.memoryTypeIndex = FindMemoryType( physicalDevice, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

        // Allocate the memory and bind it
        VK_CHECK( vkAllocateMemory( device, &allocInfo, nullptr, &memory ) );

        // TODO: assumes 0 offset.
        VK_CHECK( vkBindBufferMemory( device, buffer, memory, 0 ) );
    }

    void VulkanUtilities::CopyBuffer( VulkanRenderer* renderer, VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandPool commandPool ) {
        // Create a one-time use command buffer
        VkCommandBuffer commandBuffer = renderer->AllocateAndBeginSingleUseCommandBuffer();

        // Write the copy command.
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer( commandBuffer, source, destination, 1, &copyRegion );

        renderer->EndSingleUseCommandBuffer( commandBuffer );
    }
}