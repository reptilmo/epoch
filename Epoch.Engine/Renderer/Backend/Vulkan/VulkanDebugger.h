#pragma once

#include <vulkan/vulkan.h>

#include "../../../Types.h"

namespace Epoch {

    enum VulkanDebuggerLevel {
        ERROR,
        WARNING,
        INFO,
        TRACE
    };


    class VulkanDebugger final {
    public:

        /**
         * Creates a new vulkan debugger.
         * 
         * @param instance The Vulkan instance.
         * @param level The VulkanDebuggerLevels, ORed together.
         */
        VulkanDebugger( VkInstance instance, U64 level );

        /**
         * Default destructor.
         */
        ~VulkanDebugger();

    private:
        VkInstance _instance;
        VkDebugUtilsMessengerEXT _debugMessenger;
    };
}