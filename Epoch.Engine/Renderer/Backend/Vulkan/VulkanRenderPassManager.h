#pragma once

#include "../../RenderPassData.h"

namespace Epoch {

    class VulkanDevice;
    class VulkanRenderPass;

    class VulkanRenderPassManager {
    public:
        static void CreateRenderPass( VulkanDevice* device, const RenderPassData renderPassData );
        static VulkanRenderPass* GetRenderPass( const char* renderPassName );
        static void DestroyRenderPass( VulkanDevice* device, const char* renderPassName );

    private:
        // This is a singleton and should be treated as such.
        VulkanRenderPassManager() {}
        ~VulkanRenderPassManager() {}
    };
}