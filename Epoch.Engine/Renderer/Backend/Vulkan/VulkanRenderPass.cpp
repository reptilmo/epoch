

#include "../../../Logger.h"

#include "VulkanDevice.h"
#include "VulkanUtilities.h"
#include "VulkanRenderPass.h"

namespace Epoch {

    VkSampleCountFlagBits getSampleFlag( const U8 sampleCount ) {
        switch( sampleCount ) {
        case 1:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        case 2:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;
        case 4:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;
        case 8:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;
        case 16:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;
        case 32:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;
        case 64:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;
        default:
            Logger::Warn( "A sample count of %d is not valid. Must be 1, 2, 4, 8, 16, 32 or 64. Defaulting to 1.", sampleCount );
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        }
    }

    VkAttachmentLoadOp getLoadOp( const RenderTargetLoadOperation loadOperation ) {
        switch( loadOperation ) {
        case RenderTargetLoadOperation::CLEAR:
            return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
        case RenderTargetLoadOperation::DONT_CARE:
        default:
            return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        case RenderTargetLoadOperation::LOAD:
            return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
        }
    }

    VkAttachmentStoreOp getLoadOp( const RenderTargetStoreOperation loadOperation ) {
        switch( loadOperation ) {
        case RenderTargetStoreOperation::DONT_CARE:
        default:
            return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
        case RenderTargetStoreOperation::STORE:
            return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
        }
    }

    VulkanRenderPass::VulkanRenderPass( VulkanDevice* device, RenderPassData renderPassData ) {

        _name = renderPassData.Name.c_str();
        _device = device;

        // Main subpass
        VkSubpassDescription subpass = {};

        // Graphics or compute - in this case, graphics.
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        U32 attachmentIndex = 0;
        std::vector<VkAttachmentDescription> attachments;

        // Color attachment(s)
        std::vector<VkAttachmentReference> colorAttachmentReferences;
        for( RenderTargetOptions color : renderPassData.ColorRenderTargetOptions ) {
            VkAttachmentDescription colorAttachment = {};
            colorAttachment.format = (VkFormat)color.Format;// _swapchain->ImageFormat.format;
            colorAttachment.samples = getSampleFlag( color.SampleCount );
            colorAttachment.loadOp = getLoadOp( color.LoadOperation );
            colorAttachment.storeOp = getLoadOp( color.StoreOperation );
            colorAttachment.stencilLoadOp = getLoadOp( color.StencilLoadOperation );
            colorAttachment.stencilStoreOp = getLoadOp( color.StencilStoreOperation );
            colorAttachment.initialLayout = (VkImageLayout)color.InputLayout;
            colorAttachment.finalLayout = (VkImageLayout)color.OutputLayout;

            attachments.push_back( colorAttachment );

            // Color attachment reference
            VkAttachmentReference colorAttachmentReference = {};
            colorAttachmentReference.attachment = attachmentIndex++;
            colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentReferences.push_back( colorAttachmentReference );
        }

        // Color attachments.
        subpass.colorAttachmentCount = (U32)colorAttachmentReferences.size();
        subpass.pColorAttachments = colorAttachmentReferences.data();

        // Depth attachment
        if( renderPassData.HasDepthRenderTarget ) {
            VkAttachmentDescription depthAttachment = {};
            depthAttachment.format = _device->DepthFormat;
            depthAttachment.samples = getSampleFlag( renderPassData.DepthRenderTargetOptions.SampleCount );
            depthAttachment.loadOp = getLoadOp( renderPassData.DepthRenderTargetOptions.LoadOperation );
            depthAttachment.storeOp = getLoadOp( renderPassData.DepthRenderTargetOptions.StoreOperation );
            depthAttachment.stencilLoadOp = getLoadOp( renderPassData.DepthRenderTargetOptions.StencilLoadOperation );
            depthAttachment.stencilStoreOp = getLoadOp( renderPassData.DepthRenderTargetOptions.StencilStoreOperation );
            depthAttachment.initialLayout = (VkImageLayout)renderPassData.DepthRenderTargetOptions.InputLayout;
            depthAttachment.finalLayout = (VkImageLayout)renderPassData.DepthRenderTargetOptions.OutputLayout;

            attachments.push_back( depthAttachment );

            // Depth attachment reference
            VkAttachmentReference depthAttachmentReference = {};
            depthAttachmentReference.attachment = attachmentIndex++;
            depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            // Depth stencil data.
            subpass.pDepthStencilAttachment = &depthAttachmentReference;
        }

        // TODO: Other attachment types (input, resolve, preserve)

        // Input from a shader
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = nullptr;

        // Attachments used for multisampling colour attachments.
        subpass.pResolveAttachments = nullptr;

        // Attachments not used in this subpass, but must be preserved.
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = nullptr;

        // Render pass dependencies. TODO: make this configurable.
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // Render pass create.
        VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        renderPassCreateInfo.attachmentCount = (U32)attachments.size();
        renderPassCreateInfo.pAttachments = attachments.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &dependency;

        VK_CHECK( vkCreateRenderPass( device->LogicalDevice, &renderPassCreateInfo, nullptr, &_handle ) );
    }

    VulkanRenderPass::~VulkanRenderPass() {
        if( _handle ) {
            vkDestroyRenderPass( _device->LogicalDevice, _handle, nullptr );
            _handle = nullptr;
        }

        _device = nullptr;
    }

    void VulkanRenderPass::Begin( const RenderPassClearInfo& clearInfo, VkFramebuffer frameBuffer, VkCommandBuffer commandBuffer ) {
        VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.renderPass = _handle;
        renderPassBeginInfo.framebuffer = frameBuffer;
        renderPassBeginInfo.renderArea.offset = { (I32)clearInfo.RenderArea.X, (I32)clearInfo.RenderArea.Y };
        renderPassBeginInfo.renderArea.extent = { (U32)clearInfo.RenderArea.Z, (U32)clearInfo.RenderArea.W };

        // TODO: Clear colour and depth based on configuration.
        VkClearValue clearValues[2] = {};
        clearValues[0].color = { clearInfo.Color.X, clearInfo.Color.Y, clearInfo.Color.Z, clearInfo.Color.W };
        clearValues[1].depthStencil = { clearInfo.Depth, clearInfo.Stencil };
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass( commandBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE );
    }

    void VulkanRenderPass::End( VkCommandBuffer commandBuffer ) {
        vkCmdEndRenderPass( commandBuffer );
    }
}