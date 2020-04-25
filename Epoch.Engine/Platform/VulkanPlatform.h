#pragma once

#include <vulkan/vulkan.h>

#include "../Defines.h"
#include "../Types.h"
#include "../Containers/List.h"

namespace Epoch {

    /**
     * Contains platform-specific Vulkan functions such as surface creation. The implementation side of this code
     * provides the platform-specific logic.
     */
    class VulkanPlatform {
    public:

        /**
         * Creates a platform-specific Vulkan surface.
         * 
         * @param windowHandle A pointer to the window handle to which the surface will be bound.
         * @param instance A pointer to the Vulkan instance to be used.
         * @param ourSurface A pointer to a variable which will hold the Vulkan surface when created.
         */
        static void CreateSurface( void* windowHandle, VkInstance instance, VkSurfaceKHR* outSurface );

        /**
         * Obtains a list of required extension names for the current platform.
         * 
         * @param requiredExtensions A reference to a list of extension names which will be populated with the required extension names.
         */
        static void GetRequiredExtensions( List<const char*>& requiredExtensions );
    };
}