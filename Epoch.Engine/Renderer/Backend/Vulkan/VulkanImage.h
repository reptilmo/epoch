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

    class VulkanImage {
    public:
        void static Create( VkPhysicalDevice physicalDevice, VkDevice device, VulkanImageCreateInfo& createInfo, VulkanImage** image );
        void static CreateView( VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* view );

        ~VulkanImage();

        VkImage GetHandle() { return _imageHandle; }
        VkDeviceMemory GetMemory() { return _imageMemory; }

        const bool HasView() { return _view != nullptr; }
        VkImageView GetView() { return _view; }
    protected:
        VulkanImage( VkDevice device, VkImage imageHandle, VkDeviceMemory imageMemory, VkImageView view );
    private:
        VkDevice _device;
        VkImage _imageHandle;
        VkDeviceMemory _imageMemory;
        VkImageView _view;
    };
}