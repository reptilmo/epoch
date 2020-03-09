
#include "VulkanUtilities.h"

#include "VulkanImage.h"

namespace Epoch {

    void VulkanImage::Create( VkPhysicalDevice physicalDevice, VkDevice device, VulkanImageCreateInfo& createInfo, VulkanImage** image ) {

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
            CreateView( device, imageHandle, createInfo.Format, createInfo.ViewAspectFlags, &view );
        }
        
        *image = new VulkanImage( device, imageHandle, imageMemory, view );
    }

    void VulkanImage::CreateView( VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* view ) {
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

        VK_CHECK( vkCreateImageView( device, &viewCreateInfo, nullptr, view ) );
    }

    VulkanImage::~VulkanImage() {
        if( _view ) {
            vkDestroyImageView( _device, _view, nullptr );
            _view = nullptr;
        }

        if( _imageMemory ) {
            vkFreeMemory( _device, _imageMemory, nullptr );
            _imageMemory = nullptr;
        }

        if( _imageHandle ) {
            vkDestroyImage( _device, _imageHandle, nullptr );
            _imageHandle = nullptr;
        }

        _device = nullptr;
    }

    VulkanImage::VulkanImage( VkDevice device, VkImage imageHandle, VkDeviceMemory imageMemory, VkImageView view ) {
        _device = device;
        _imageHandle = imageHandle;
        _imageMemory = imageMemory;
        _view = view;
    }
}