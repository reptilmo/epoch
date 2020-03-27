#pragma once

#include <vulkan/vulkan.h>
#include "../../RenderPassData.h"
#include "../../../Math/Vector4.h"

namespace Epoch {

    class IRenderTarget;
    class VulkanDevice;

    struct RenderPassClearInfo {
        Vector4 Color;
        Vector4 RenderArea;
        F32 Depth;
        U32 Stencil;
    };

    class VulkanRenderPass {
    public:
        //IRenderTarget* ColorRenderTarget = nullptr;
        //IRenderTarget* DepthRenderTarget = nullptr;
    public:
        VulkanRenderPass( VulkanDevice* device, RenderPassData renderPassData );
        ~VulkanRenderPass();

        void Begin( const RenderPassClearInfo& clearInfo, VkFramebuffer frameBuffer, VkCommandBuffer commandBuffer );

        void End( VkCommandBuffer commandBuffer );

        const char* GetName() const { return _name; }
        VkRenderPass GetHandle() { return _handle; }
    private:
        const char* _name;
        VkRenderPass _handle;
        VulkanDevice* _device;
    };
}