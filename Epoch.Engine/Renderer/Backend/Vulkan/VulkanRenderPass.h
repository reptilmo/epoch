#pragma once

#include <vulkan/vulkan.h>
#include "../../RenderPassData.h"
#include "../../../Math/Vector4.h"

namespace Epoch {

    class IRenderTarget;
    class VulkanDevice;
    class VulkanCommandBuffer;

    struct RenderPassClearInfo {
        Vector4 Color = { 0,0,0,0 };
        Vector4 RenderArea = { 0,0,0,0 };
        F32 Depth = 0;
        U32 Stencil = 0;
    };

    class VulkanRenderPass {
    public:
        //IRenderTarget* ColorRenderTarget = nullptr;
        //IRenderTarget* DepthRenderTarget = nullptr;
    public:
        VulkanRenderPass( VulkanDevice* device, RenderPassData renderPassData );
        ~VulkanRenderPass();

        void Begin( const RenderPassClearInfo& clearInfo, VkFramebuffer frameBuffer, VulkanCommandBuffer* commandBuffer );

        void End( VulkanCommandBuffer* commandBuffer );

        const char* GetName() const { return _name; }
        VkRenderPass GetHandle() { return _handle; }
    private:
        const char* _name;
        VkRenderPass _handle;
        VulkanDevice* _device;
    };
}