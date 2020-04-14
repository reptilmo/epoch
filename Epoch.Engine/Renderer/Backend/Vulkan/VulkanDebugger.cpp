#include "../../../Defines.h"
#include "../../../Logger.h"

#include "VulkanDebugger.h"

namespace Epoch {

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData ) {

        switch( messageSeverity ) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            Logger::Error( pCallbackData->pMessage );
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Logger::Warn( pCallbackData->pMessage );
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Logger::Log( pCallbackData->pMessage );
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            Logger::Trace( pCallbackData->pMessage );
            break;
        }

        return VK_FALSE;
    }

    VulkanDebugger::VulkanDebugger( VkInstance instance, U64 level ) {
        _instance = instance;

        // Error severity is always shown. The others are tacked on as they are needed.
        U32 severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        if( ( level & VulkanDebuggerLevel::WARNING ) == VulkanDebuggerLevel::WARNING ) {
            severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        }
        if( ( level & VulkanDebuggerLevel::INFO ) == VulkanDebuggerLevel::INFO ) {
            severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        }
        if( ( level & VulkanDebuggerLevel::TRACE ) == VulkanDebuggerLevel::TRACE ) {
            severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        }

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debugCreateInfo.messageSeverity = severity;// VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        debugCreateInfo.pUserData = this;

        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( _instance, "vkCreateDebugUtilsMessengerEXT" );
        ASSERT_MSG( func, "Failed to create debug messenger!" );
        func( _instance, &debugCreateInfo, nullptr, &_debugMessenger );
    }

    VulkanDebugger::~VulkanDebugger() {
        if( _debugMessenger ) {
            PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( _instance, "vkDestroyDebugUtilsMessengerEXT" );
            // ASSERT_MSG( func, "Failed to destroy debug messenger!" );
            func( _instance, _debugMessenger, nullptr );
        }
    }
}