
#include "../../../Math/TMath.h"
#include "../../../Logger.h"

#include "VulkanRenderer.h"
#include "VulkanDevice.h"
#include "VulkanUtilities.h"
#include "VulkanTexture.h"
#include "VulkanImage.h"
#include "VulkanRenderPassManager.h"
#include "VulkanRenderPass.h"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanSwapchain.h"

namespace Epoch {


    VulkanSwapchain::VulkanSwapchain( VulkanDevice* device, VkSurfaceKHR surface, void* windowHandle, U32 width, U32 height ) {
        _device = device;
        _surface = surface;
        Extent = { width, height };
        createInternal();
    }

    VulkanSwapchain::~VulkanSwapchain() {
        destroyInternal();

        _surface = nullptr;
        _device = nullptr;
    }

    void VulkanSwapchain::Recreate( U32 width, U32 height ) {
        Extent = { width, height };

        VK_CHECK( vkDeviceWaitIdle( _device->LogicalDevice ) );

        destroyInternal();
        createInternal();
    }

    const bool VulkanSwapchain::AcquireNextImageIndex( const U64 timeoutNS, VulkanSemaphore* imageAvailableSemaphore, VulkanFence* fence, U32* imageIndex ) {
        VkFence fenceHandle = nullptr;
        if( fence ) {
            fenceHandle = fence->GetHandle();
        }
        VkResult result = vkAcquireNextImageKHR( _device->LogicalDevice, _handle, timeoutNS, imageAvailableSemaphore->GetHandle(), fenceHandle, imageIndex );
        if( result == VkResult::VK_ERROR_OUT_OF_DATE_KHR ) {
            // Trigger swapchain recreation, then boot out of the render loop.
            Extent2D extent = _device->FramebufferSize;
            Recreate( extent.Width, extent.Height );
            return false;
        } else if( result != VkResult::VK_SUCCESS && result != VkResult::VK_SUBOPTIMAL_KHR ) {

            // This is fatal and should crash the application.
            Logger::Fatal( "Failed to acquire swapchain image!" );
            return false;
        }

        return true;
    }

    void VulkanSwapchain::Present( VkQueue graphicsQueue, VkQueue presentQueue, VulkanSemaphore* renderCompleteSemaphore, const U32 presentImageIndex ) {

        // Return the image to the swapchain for presentation
        VkSemaphore renderCompleteSem = renderCompleteSemaphore->GetHandle();
        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderCompleteSem;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &_handle;
        presentInfo.pImageIndices = &presentImageIndex;
        presentInfo.pResults = nullptr;

        VkResult result = vkQueuePresentKHR( _device->PresentationQueue, &presentInfo );
        if( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ) {
            // Swapchain is out of date, suboptimal or a framebuffer resize has occurred. Trigger swapchain recreation.
            Extent2D extent = _device->FramebufferSize;
            Recreate( extent.Width, extent.Height );
        } else if( result != VkResult::VK_SUCCESS ) {
            Logger::Fatal( "Failed to present swap chain image!" );
        }

        // Increment (and loop) the index.
        _currentFrameIndex = ( _currentFrameIndex + 1 ) % MaxFramesInFlight;
    }

    void VulkanSwapchain::RegenerateFramebuffers() {

        U64 swapchainImageCount = _swapchainImages.size();

        // Framebuffers
        _swapchainFramebuffers.resize( swapchainImageCount );

        for( U32 i = 0; i < swapchainImageCount; ++i ) {
            U32 attachmentCount = 2; // TODO: Make this dynamic based on currently configured attachments.
            VkImageView attachments[] = {
                _swapchainImageViews[i],
                ( (VulkanImage*)_depthAttachment->GetImage() )->GetView()
            };

            VkFramebufferCreateInfo framebufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
            // TODO: use passed-in name
            framebufferCreateInfo.renderPass = VulkanRenderPassManager::GetRenderPass( "RenderPass.Default" )->GetHandle();
            framebufferCreateInfo.attachmentCount = attachmentCount;
            framebufferCreateInfo.pAttachments = attachments;
            framebufferCreateInfo.width = Extent.width;
            framebufferCreateInfo.height = Extent.height;
            framebufferCreateInfo.layers = 1;

            VK_CHECK( vkCreateFramebuffer( _device->LogicalDevice, &framebufferCreateInfo, nullptr, &_swapchainFramebuffers[i] ) );
        }
    }

    void VulkanSwapchain::destroyInternal() {

        for( U64 i = 0; i < _swapchainImages.size(); ++i ) {
            vkDestroyFramebuffer( _device->LogicalDevice, _swapchainFramebuffers[i], nullptr );
        }

        if( _depthAttachment ) {
            delete _depthAttachment;
            _depthAttachment = nullptr;
        }

        for( auto scImageView : _swapchainImageViews ) {
            vkDestroyImageView( _device->LogicalDevice, scImageView, nullptr );
        }
        _swapchainImages.clear();

        vkDestroySwapchainKHR( _device->LogicalDevice, _handle, nullptr );
    }

    void VulkanSwapchain::createInternal() {
        VulkanSwapchainSupportDetails swapchainSupport = _device->SwapchainSupport;
        VkSurfaceCapabilitiesKHR capabilities = swapchainSupport.Capabilities;

        // Choose a swap surface format
        bool found = false;
        for( auto format : swapchainSupport.Formats ) {

            // Preferred formats 
            if( format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
                ImageFormat = format;
                found = true;
                break;
            }
        }

        if( !found ) {
            ImageFormat = swapchainSupport.Formats[0];
        }

        // Choose a present mode
        VkPresentModeKHR presentMode;
        found = false;
        for( auto mode : swapchainSupport.PresentationModes ) {

            // If preferred mode is available, use it
            if( mode == VK_PRESENT_MODE_MAILBOX_KHR ) {
                presentMode = mode;
                found = true;
            }
        }

        if( !found ) {
            presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        // Swapchain extent
        if( capabilities.currentExtent.width != U32_MAX ) {
            Extent = capabilities.currentExtent;
        } else {

            // Clamp to a value allowed by the GPU.
            Extent.width = TMath::ClampU32( Extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
            Extent.height = TMath::ClampU32( Extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
        }

        U32 imageCount = capabilities.minImageCount + 1;

        if( capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount ) {
            imageCount = capabilities.maxImageCount;
        }

        // Swapchain create info
        VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = _surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = ImageFormat.format;
        swapchainCreateInfo.imageColorSpace = ImageFormat.colorSpace;
        swapchainCreateInfo.imageExtent = Extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Setup the queue family indices
        if( _device->GraphicsFamilyQueueIndex != _device->PresentationFamilyQueueIndex ) {
            U32 queueFamilyIndices[] = { (U32)_device->GraphicsFamilyQueueIndex, (U32)_device->PresentationFamilyQueueIndex };
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        swapchainCreateInfo.preTransform = capabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = nullptr;

        VK_CHECK( vkCreateSwapchainKHR( _device->LogicalDevice, &swapchainCreateInfo, nullptr, &_handle ) );

        // Images
        U32 swapchainImageCount = 0;
        vkGetSwapchainImagesKHR( _device->LogicalDevice, _handle, &swapchainImageCount, nullptr );
        _swapchainImages.resize( swapchainImageCount );
        _swapchainImageViews.resize( swapchainImageCount );
        vkGetSwapchainImagesKHR( _device->LogicalDevice, _handle, &swapchainImageCount, _swapchainImages.data() );

        // Views
        for( U32 i = 0; i < swapchainImageCount; ++i ) {
            VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            viewInfo.image = _swapchainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = ImageFormat.format;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            VK_CHECK( vkCreateImageView( _device->LogicalDevice, &viewInfo, nullptr, &_swapchainImageViews[i] ) );
        }

        // Depth resources
        _depthFormat = VulkanUtilities::FindDepthFormat( _device->PhysicalDevice );
        VulkanImageCreateInfo depthImageCreateInfo;
        VkExtent2D extent = Extent;
        depthImageCreateInfo.Width = extent.width;
        depthImageCreateInfo.Height = extent.height;
        depthImageCreateInfo.Type = VK_IMAGE_TYPE_2D;
        depthImageCreateInfo.Format = _depthFormat;
        depthImageCreateInfo.Tiling = VK_IMAGE_TILING_OPTIMAL;
        depthImageCreateInfo.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImageCreateInfo.Properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        depthImageCreateInfo.CreateView = true;
        depthImageCreateInfo.ViewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;

        // TODO: Streamline this process.
        VulkanImage* depth;
        VulkanImage::Create( _device, depthImageCreateInfo, &depth );
        _depthAttachment = new VulkanTexture( depth, "depth", true );
    }
}