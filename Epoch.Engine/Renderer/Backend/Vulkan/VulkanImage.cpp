
#include "../../../Logger.h"

#include "VulkanUtilities.h"
#include "VulkanRenderer.h"
#include "VulkanImage.h"

namespace Epoch {

    void VulkanImage::Create( VulkanRenderer* renderer, VulkanImageCreateInfo& createInfo, VulkanImage** image ) {

        VkPhysicalDevice physicalDevice = renderer->GetPhysicalDeviceHandle();
        VkDevice device = renderer->GetDeviceHandle();

        // Create image
        VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageCreateInfo.imageType = createInfo.Type;
        imageCreateInfo.extent.width = createInfo.Width;
        imageCreateInfo.extent.height = createInfo.Height;
        imageCreateInfo.extent.depth = 1; // TODO: Support configurable depth.
        imageCreateInfo.mipLevels = 1; // TODO: Support mip mapping
        imageCreateInfo.arrayLayers = 1; // TODO: Support number of layers in the image.
        imageCreateInfo.format = createInfo.Format;
        imageCreateInfo.tiling = createInfo.Tiling;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = createInfo.Usage;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Configurable sample count.
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode.

        VkImage imageHandle;
        VK_CHECK( vkCreateImage( device, &imageCreateInfo, nullptr, &imageHandle ) );

        // Query memory requirements.
        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements( device, imageHandle, &memoryRequirements );

        // Allocate memory
        VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocateInfo.allocationSize = memoryRequirements.size;
        allocateInfo.memoryTypeIndex = VulkanUtilities::FindMemoryType( physicalDevice, memoryRequirements.memoryTypeBits, createInfo.Properties );

        VkDeviceMemory imageMemory;
        VK_CHECK( vkAllocateMemory( device, &allocateInfo, nullptr, &imageMemory ) );

        // Bind the memory
        VK_CHECK( vkBindImageMemory( device, imageHandle, imageMemory, 0 ) ); // TODO: configurable memory offset.

        VkImageView view = nullptr;
        if( createInfo.CreateView ) {
            CreateView( renderer, imageHandle, createInfo.Format, createInfo.ViewAspectFlags, &view );
        }

        *image = new VulkanImage( renderer, createInfo.Width, createInfo.Height, imageHandle, imageMemory, view );
    }

    void VulkanImage::CreateView( VulkanRenderer* renderer, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* view ) {
        VkImageViewCreateInfo viewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewCreateInfo.image = image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: Make configurable.
        viewCreateInfo.format = format;
        viewCreateInfo.subresourceRange.aspectMask = aspectFlags;

        // TODO: Make configurable
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        VK_CHECK( vkCreateImageView( renderer->GetDeviceHandle(), &viewCreateInfo, nullptr, view ) );
    }

    void VulkanImage::TransitionLayout( VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout ) {
        VkCommandBuffer commandBuffer = _renderer->AllocateAndBeginSingleUseCommandBuffer();

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = (VkImageLayout)oldLayout;
        barrier.newLayout = (VkImageLayout)newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = _imageHandle;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            Logger::Fatal( "unsupported layout transition!" );
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        _renderer->EndSingleUseCommandBuffer( commandBuffer );
    }

    void VulkanImage::CopyFromBuffer( VkBuffer buffer ) {
        VkCommandBuffer commandBuffer = _renderer->AllocateAndBeginSingleUseCommandBuffer();

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            _width,
            _height,
            1
        };

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            _imageHandle,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        _renderer->EndSingleUseCommandBuffer( commandBuffer );
    }

    VulkanImage::~VulkanImage() {
        VkDevice device = _renderer->GetDeviceHandle();
        if( _view ) {
            vkDestroyImageView( device, _view, nullptr );
            _view = nullptr;
        }

        if( _imageMemory ) {
            vkFreeMemory( device, _imageMemory, nullptr );
            _imageMemory = nullptr;
        }

        if( _imageHandle ) {
            vkDestroyImage( device, _imageHandle, nullptr );
            _imageHandle = nullptr;
        }

        _renderer = nullptr;
    }

    VulkanImage::VulkanImage( VulkanRenderer* renderer, U32 width, U32 height, VkImage imageHandle, VkDeviceMemory imageMemory, VkImageView view ) {
        _renderer = renderer;
        _width = width;
        _height = height;
        _imageHandle = imageHandle;
        _imageMemory = imageMemory;
        _view = view;
    }
}