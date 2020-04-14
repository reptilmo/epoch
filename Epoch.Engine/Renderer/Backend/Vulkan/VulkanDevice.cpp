
#include <vector>

#include "../../../Types.h"
#include "../../../Logger.h"

#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanQueue.h"
#include "VulkanDevice.h"


namespace Epoch {

    VulkanDevice::VulkanDevice( VkInstance instance, const std::vector<const char*>& requiredValidationLayers, VkSurfaceKHR surface ) {
        _instance = instance;
        _surface = surface;

        selectPhysicalDevice();
        createLogicalDevice( requiredValidationLayers );
        createQueues();
        createCommandPool();

        if( !detectDepthFormat() ) {
            Logger::Fatal( "Unable to find a supported depth format!" );
        }
    }

    VulkanDevice::~VulkanDevice() {

        if( CommandPool ) {
            vkDestroyCommandPool( LogicalDevice, CommandPool->GetHandle(), nullptr );
        }

        if( LogicalDevice ) {
            vkDestroyDevice( LogicalDevice, nullptr );
            LogicalDevice = nullptr;
        }

        // There is no vkDestroyPhysicalDevice.
        PhysicalDevice = nullptr;
        GraphicsFamilyQueueIndex = -1;
        PresentationFamilyQueueIndex = -1;

        // Queues are not destroyed either
        GraphicsQueue = nullptr;
        PresentationQueue = nullptr;
    }

    void VulkanDevice::WaitIdle() const {
        VK_CHECK( vkDeviceWaitIdle( LogicalDevice ) );
    }

    VulkanCommandBuffer* VulkanDevice::AllocateAndBeginSingleUseCommandBuffer() {
        VulkanCommandBuffer* commandBuffer = CommandPool->AllocateCommandBuffer( true );
        commandBuffer->Begin( true, false, false );
        return commandBuffer;
    }

    void VulkanDevice::EndSingleUseCommandBuffer( VulkanCommandBuffer* commandBuffer ) {
        commandBuffer->End();
        GraphicsQueue->Submit( commandBuffer, nullptr, 0, nullptr, true );
        CommandPool->FreeCommandBuffer( commandBuffer );
    }

    void VulkanDevice::RequerySupport() {
        querySwapchainSupport( PhysicalDevice );
        detectDepthFormat();
    }

    void VulkanDevice::selectPhysicalDevice() {
        U32 deviceCount = 0;
        vkEnumeratePhysicalDevices( _instance, &deviceCount, nullptr );
        if( deviceCount == 0 ) {
            Logger::Fatal( "No supported physical device were found." );
        }
        std::vector<VkPhysicalDevice> devices( deviceCount );
        vkEnumeratePhysicalDevices( _instance, &deviceCount, devices.data() );

        for( U32 i = 0; i < deviceCount; ++i ) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties( devices[i], &properties );

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures( devices[i], &features );

            VkPhysicalDeviceMemoryProperties memoryProperties;
            vkGetPhysicalDeviceMemoryProperties( devices[i], &memoryProperties );

            if( physicalDeviceMeetsRequirements( devices[i], &properties, &features ) ) {

                // Print some info about the GPU
                Logger::Log( "Selected device: %s", properties.deviceName );
                switch( properties.deviceType ) {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    Logger::Log( "Unknown GPU type" );
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    Logger::Log( "Integrated GPU" );
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    Logger::Log( "Descrete GPU" );
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    Logger::Log( "Virtual GPU" );
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    Logger::Log( "CPU 'GPU' type" );
                    break;
                }

                // Memory info
                for( U32 i = 0; i < memoryProperties.memoryHeapCount; ++i ) {
                    F32 memorySizeMiB = ( ( (F32)memoryProperties.memoryHeaps[i].size ) / 1024.0f / 1024.0f );
                    if( memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ) {
                        Logger::Log( "Local GPU memory: %.2f MiB", memorySizeMiB );
                    } else {
                        Logger::Log( "Shared System memory: %.2f MiB", memorySizeMiB );
                    }
                }

                PhysicalDevice = devices[i];
                return;
            }
        }

        Logger::Fatal( "No devices found which meet application requirements" );
    }

    const bool VulkanDevice::physicalDeviceMeetsRequirements( VkPhysicalDevice physicalDevice, const VkPhysicalDeviceProperties* properties, const VkPhysicalDeviceFeatures* features ) {
        I32 graphicsQueueIndex = -1;
        I32 presentationQueueIndex = -1;
        detectQueueFamilyIndices( physicalDevice, &graphicsQueueIndex, &presentationQueueIndex );
        querySwapchainSupport( physicalDevice );

        bool supportsRequiredQueueFamilies = ( graphicsQueueIndex != -1 ) && ( presentationQueueIndex != -1 );

        // Device extension support. Supported/available extensions
        U32 extensionCount = 0;
        vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, nullptr );
        std::vector<VkExtensionProperties> availableExtensions( extensionCount );
        vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, availableExtensions.data() );

        // Required extensions
        std::vector<const char*> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        bool success = true;
        for( U64 i = 0; i < requiredExtensions.size(); ++i ) {
            bool found = false;
            for( U64 j = 0; j < extensionCount; ++j ) {
                if( strcmp( requiredExtensions[i], availableExtensions[j].extensionName ) == 0 ) {
                    found = true;
                    break;
                }
            }

            if( !found ) {
                success = false;
                break;
            }
        }

        bool swapChainMeetsRequirements = false;
        if( supportsRequiredQueueFamilies ) {
            swapChainMeetsRequirements = SwapchainSupport.Formats.size() > 0 && SwapchainSupport.PresentationModes.size() > 0;
        }

        // NOTE: Could also look for discrete GPU. We could score and rank them based on features and capabilities.
        return supportsRequiredQueueFamilies && swapChainMeetsRequirements && features->samplerAnisotropy;
    }

    void VulkanDevice::detectQueueFamilyIndices( VkPhysicalDevice physicalDevice, I32* graphicsQueueIndex, I32* presentationQueueIndex ) {
        U32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, nullptr );
        std::vector<VkQueueFamilyProperties> familyProperties( queueFamilyCount );
        vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, familyProperties.data() );

        for( U32 i = 0; i < queueFamilyCount; ++i ) {

            // Does it support the graphics queue?
            if( familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
                *graphicsQueueIndex = i;
            }

            VkBool32 supportsPresentation = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR( physicalDevice, i, _surface, &supportsPresentation );
            if( supportsPresentation ) {
                *presentationQueueIndex = i;
            }
        }
    }

    void VulkanDevice::querySwapchainSupport( VkPhysicalDevice physicalDevice ) {

        // Capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice, _surface, &SwapchainSupport.Capabilities );

        // Surface formats
        U32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, _surface, &formatCount, nullptr );
        if( formatCount != 0 ) {
            SwapchainSupport.Formats.resize( formatCount );
            vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, _surface, &formatCount, SwapchainSupport.Formats.data() );
        }

        // Presentation modes
        U32 presentationModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, _surface, &presentationModeCount, nullptr );
        if( presentationModeCount != 0 ) {
            SwapchainSupport.PresentationModes.resize( presentationModeCount );
            vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, _surface, &presentationModeCount, SwapchainSupport.PresentationModes.data() );
        }
    }

    void VulkanDevice::createLogicalDevice( const std::vector<const char*>& requiredValidationLayers ) {
        I32 graphicsQueueIndex = -1;
        I32 presentationQueueIndex = -1;
        detectQueueFamilyIndices( PhysicalDevice, &graphicsQueueIndex, &presentationQueueIndex );

        // If the queue indices are the same, only one queue needs to be created.
        bool presentationSharesGraphicsQueue = graphicsQueueIndex == presentationQueueIndex;

        std::vector<U32> indices;
        indices.push_back( graphicsQueueIndex );
        if( !presentationSharesGraphicsQueue ) {
            indices.push_back( presentationQueueIndex );
        }

        // Device queues
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos( indices.size() );
        for( U32 i = 0; i < (U32)indices.size(); ++i ) {
            queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfos[i].queueFamilyIndex = indices[i];
            queueCreateInfos[i].queueCount = 1;
            queueCreateInfos[i].flags = 0;
            queueCreateInfos[i].pNext = nullptr;
            F32 queuePriority = 1.0f;
            queueCreateInfos[i].pQueuePriorities = &queuePriority;
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE; // Request anistrophy

        VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = (U32)indices.size();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = 1;
        deviceCreateInfo.pNext = nullptr;
        const char* requiredExtensions[1] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions;

#if _DEBUG
        deviceCreateInfo.enabledLayerCount = (U32)requiredValidationLayers.size();
        deviceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
#else
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.ppEnabledLayerNames = nullptr;
#endif

        // Create the device
        VK_CHECK( vkCreateDevice( PhysicalDevice, &deviceCreateInfo, nullptr, &LogicalDevice ) );

        // Save off the queue family indices
        GraphicsFamilyQueueIndex = graphicsQueueIndex;
        PresentationFamilyQueueIndex = presentationQueueIndex;
    }

    void VulkanDevice::createQueues() {

        GraphicsQueue = new VulkanQueue( this, GraphicsFamilyQueueIndex );
        PresentationQueue = new VulkanQueue( this, PresentationFamilyQueueIndex );
    }

    void VulkanDevice::createCommandPool() {
        CommandPool = new VulkanCommandPool( this, GraphicsFamilyQueueIndex, true, false, false );
    }

    const bool VulkanDevice::detectDepthFormat() {

        // Format candidates
        const U64 candidateCount = 3;
        VkFormat candidates[candidateCount] = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };

        U32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        for( U64 i = 0; i < candidateCount; ++i ) {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties( PhysicalDevice, candidates[i], &properties );

            if( ( properties.linearTilingFeatures & flags ) == flags ) {
                DepthFormat = candidates[i];
                return true;
            } else if( ( properties.optimalTilingFeatures & flags ) == flags ) {
                DepthFormat = candidates[i];
                return true;
            }
        }

        return false;
    }
}