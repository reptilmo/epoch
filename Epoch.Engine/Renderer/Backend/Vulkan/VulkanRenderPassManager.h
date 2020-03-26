#pragma once

#include <vulkan/vulkan.h>

namespace Epoch {

    class VulkanDevice;

    class VulkanRenderPassManager {
    public:
        // TODO: handle this more internally via a render pass class.
        static const bool CreateRenderPass( VulkanDevice* device, const VkRenderPassCreateInfo& info );
        static VkRenderPass GetRenderPass( const char* renderPassName );
        static void DestroyRenderPass( VulkanDevice* device, const char* renderPassName );

    private:
        // This is a singleton and should be treated as such.
        VulkanRenderPassManager() {}
        ~VulkanRenderPassManager() {}

    };
}