#pragma once

#include <vulkan/vulkan.h>
#include "Types.h"
#include "Events/Event.h"

struct GLFWwindow;

namespace Epoch {

    class Engine;

    struct WindowResizedEvent : public Event {
        friend class Platform;

        U32 PreviousWidth;
        U32 PreviousHeight;
        U32 NewWidth;
        U32 NewHeight;

        WindowResizedEvent( Event event ) : Event( event.Type, event.Sender ) {}
    private:
        WindowResizedEvent( void* sender, U32 prevWidth, U32 prevHeight, U32 newWidth, U32 newHeight )
            : Event( EventType::WINDOW_RESIZED, sender ) {

            PreviousWidth = prevWidth;
            PreviousHeight = prevHeight;
            NewWidth = newWidth;
            NewHeight = newHeight;
        }
    };

    class Platform {
    public:
        Platform( Engine* engine, const char* applicationName );
        ~Platform();

        GLFWwindow* GetWindow() { return _window; }

        Extent2D GetFramebufferExtent();

        void GetRequiredExtensions( U32* extensionCount, const char*** extensionNames );

        void CreateSurface( VkInstance instance, VkSurfaceKHR* surface );

        const bool StartGameLoop();

        void WaitEvents();
    private:
        static void onFrameBufferResize( GLFWwindow* window, I32 width, I32 height );
    private:
        Engine* _engine;
        GLFWwindow* _window;
    };
}