#pragma once

#include <vulkan/vulkan.h>

#include "../../../Types.h"

namespace Epoch {

    struct VulkanImageCreateInfo {
        U32 Width;
        U32 Height;
        VkImageType Type = VkImageType::VK_IMAGE_TYPE_2D;
        VkFormat Format;
        VkImageTiling Tiling;
        VkImageUsageFlags Usage;
        VkMemoryPropertyFlags Properties;

        // View info
        bool CreateView = false;
        VkImageAspectFlags ViewAspectFlags;
    };

    class VulkanDevice;

    class VulkanImage {
    public:
        void static Create( VulkanDevice* device, VulkanImageCreateInfo& createInfo, VulkanImage** image );
        void static CreateView( VulkanDevice* device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* view );

    public:
        void TransitionLayout( VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout );
        void CopyFromBuffer( VkBuffer buffer );

        ~VulkanImage();

        VkImage GetHandle() { return _imageHandle; }
        VkDeviceMemory GetMemory() { return _imageMemory; }

        const bool HasView() { return _view != nullptr; }
        VkImageView GetView() { return _view; }
    protected:
        VulkanImage( VulkanDevice* device, U32 width, U32 height, VkImage imageHandle, VkDeviceMemory imageMemory, VkImageView view );
    private:
        U32 _width, _height;
        VulkanDevice* _device;
        VkImage _imageHandle;
        VkDeviceMemory _imageMemory;
        VkImageView _view;
    };
}