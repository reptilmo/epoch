
#include <vulkan/vulkan.h>

#include "../../../Defines.h"
#include "../../../Memory/Memory.h"
#include "../../../Assets/Image/ImageUtilities.h"

#include "VulkanRenderer.h"
#include "VulkanUtilities.h"
#include "VulkanInternalBuffer.h"
#include "VulkanImage.h"
#include "VulkanTexture.h"

namespace Epoch {

    VulkanTexture::VulkanTexture( VulkanDevice* device, const char* name, const char* path ) {
        _device = device;
        _name = name;


        I32 width, height, channelCount;
        byte* pixels = ImageUtilities::LoadImage( path, &width, &height, &channelCount );
        ASSERT_MSG( pixels, "Unable to load image!" );

        VkDeviceSize imageSize = (U64)width * (U64)height * 4;

        // Create a staging buffer and load data into it.
        VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        VulkanInternalBuffer staging( _device, imageSize, usage, flags, true );

        // Lock, copy, unlock
        void* data = staging.LockMemory( 0, imageSize, 0 );
        TMemory::Memcpy( data, pixels, imageSize );
        staging.UnlockMemory();

        // Clean up image data.
        TMemory::Free( pixels );

        VulkanImageCreateInfo textureImageCreateInfo = {};
        textureImageCreateInfo.Width = width;
        textureImageCreateInfo.Height = height;
        textureImageCreateInfo.Format = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
        textureImageCreateInfo.Tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        textureImageCreateInfo.Usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
        textureImageCreateInfo.Properties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        textureImageCreateInfo.CreateView = true;
        textureImageCreateInfo.ViewAspectFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
        VulkanImage::Create( _device, textureImageCreateInfo, &_textureImage );

        // Transition the layout from whatever it is currently to optimal for recieving data.
        _textureImage->TransitionLayout( textureImageCreateInfo.Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

        // Copy the data from the buffer.
        _textureImage->CopyFromBuffer( staging.GetHandle() );

        // Transition from optimal for data reciept to shader-read-only optimal layout.
        _textureImage->TransitionLayout( textureImageCreateInfo.Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
    }

    VulkanTexture::~VulkanTexture() {
        if( _textureImage ) {
            delete _textureImage;
            _textureImage = nullptr;
        }

        _device = nullptr;
    }
}