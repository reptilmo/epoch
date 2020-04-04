#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "../Logger.h"
#include "../Engine.h"

// TODO: move this to renderer layer
#include "../Renderer/Backend/Vulkan/VulkanUtilities.h"

#include "TGLFWWindow.h"
#include "Platform.h"

namespace Epoch {

    IWindow* Platform::CreateWindow( const char* applicationName, U32 width, U32 height ) {
        return new TGLFWWindow( applicationName, width, height );
    }

    Platform::Platform( Engine* engine, const char* applicationName ) {
        Logger::Trace( "Initializing platform layer..." );
        _engine = engine;

        glfwInit();

        // If vulkan
        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

        // TODO: stop-gap for now
        _window = glfwCreateWindow( 1280, 720, applicationName, nullptr, nullptr );
        glfwSetWindowUserPointer( _window, this );
        glfwSetFramebufferSizeCallback( _window, Platform::onFrameBufferResize );

    }
    Platform::~Platform() {
        if( _window ) {
            glfwDestroyWindow( _window );
        }

        glfwTerminate();
    }

    Extent2D Platform::GetFramebufferExtent() {
        Extent2D extents;
        glfwGetFramebufferSize( _window, &extents.Width, &extents.Height );
        return extents;
    }

    void Platform::GetRequiredExtensions( U32* extensionCount, const char*** extensionNames ) {
        *extensionNames = glfwGetRequiredInstanceExtensions( extensionCount );
    }

    void Platform::CreateSurface( VkInstance instance, VkSurfaceKHR* surface ) {
        VK_CHECK( glfwCreateWindowSurface( instance, _window, nullptr, surface ) );
    }

    const bool Platform::StartGameLoop() {
        while( !glfwWindowShouldClose( _window ) ) {
            glfwPollEvents();

            if( !_engine->OnLoop( 0 ) ) {
                Logger::Fatal( "Engine loop failed! See logs for details." );
            }
        }

        WaitEvents();

        return true;
    }

    void Platform::WaitEvents() {
        glfwWaitEvents();
    }

    void Platform::onFrameBufferResize( GLFWwindow* window, I32 width, I32 height ) {
        WindowResizedEvent resizeEvent( window, 0, 0, (U32)width, (U32)height );
        resizeEvent.Post();
    }
}