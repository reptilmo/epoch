#include "../VulkanPlatform.h"
#ifdef PLATFORM_WINDOWS

#include <Windows.h>
#include <vulkan/vulkan_win32.h>

#include "../../Renderer/Backend/Vulkan/VulkanUtilities.h"

namespace Epoch {

    void VulkanPlatform::CreateSurface( void* windowHandle, VkInstance instance, VkSurfaceKHR* outSurface ) {
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
        surfaceCreateInfo.hinstance = GetModuleHandle( nullptr );
        surfaceCreateInfo.hwnd = (HWND)windowHandle;
        VK_CHECK( vkCreateWin32SurfaceKHR( instance, &surfaceCreateInfo, nullptr, outSurface ) );
    }

    void VulkanPlatform::GetRequiredExtensions( List<const char*>& requiredExtensions ) {

        // Surface extension required by all OSes
        requiredExtensions.Add( "VK_KHR_surface" );

        // Windows-specific surface extension.
        requiredExtensions.Add( "VK_KHR_win32_surface" );

        // The debug utility extension.
        requiredExtensions.Add( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
    }
}

#endif