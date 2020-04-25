#pragma once

#include <vulkan/vulkan.h>
#include "../Types.h"
#include "../Events/Event.h"

struct GLFWwindow;

namespace Epoch {

    class IWindow;
    class Engine;
    class Clock;

    /**
     * A specialized event used to signal a window resize having occurred.
     */
    struct WindowResizedEvent final : public Event {
        friend class Platform;

        /**
         * The previous width of the window in pixels.
         */
        U32 PreviousWidth;

        /**
         * The previous height of the window in pixels.
         */
        U32 PreviousHeight;

        /**
         * The new width of the window in pixels.
         */
        U32 NewWidth;

        /**
         * The new height of the window in pixels.
         */
        U32 NewHeight;

        /**
         * Specialized conversion constructor.
         */
        WindowResizedEvent( Event event ) : Event( event.Type, event.Sender ) {
            PreviousWidth = 0;
            PreviousHeight = 0;
            NewWidth = 0;
            NewHeight = 0;
        }
    private:
        WindowResizedEvent( void* sender, U32 prevWidth, U32 prevHeight, U32 newWidth, U32 newHeight )
            : Event( EventType::WINDOW_RESIZED, sender ) {

            PreviousWidth = prevWidth;
            PreviousHeight = prevHeight;
            NewWidth = newWidth;
            NewHeight = newHeight;
        }
    };

    /**
     * Represents the platform layer, which abstracts OS-specific details such as the
     * windowing system.
     */
    class Platform final {
    public:

        /**
         * Creates and returns a pointer to a new window.
         *
         * @param applicationName The name of the application. Used in the window title.
         * @param width The number of pixels wide the client area of the window will be.
         * @param height The number of pixels high the client area of the window will be.
         *
         * @returns A pointer to the window.
         */
        static IWindow* CreateWindow( const char* applicationName, U32 width, U32 height );

        static U32 Cycles();
        static U64 Cycles64();
    public:

        /**
         * Creates a new Platform instance.
         *
         * @param engine A pointer to the Engine instance which owns this object.
         * @param applicationName The name of the application. Used in the window title.
         */
        Platform( Engine* engine, const char* applicationName );

        /**
         *Default destructor.
         */
        ~Platform();

        /**
         * Returns a pointer to the window handle.
         *
         * @returns A pointer to the window handle.
         */
        GLFWwindow* GetWindow() { return _window; }

        /**
         * Returns the extents (visible size in pixels) of the framebuffer.
         *
         * @returns A the framebuffer extents.
         */
        Extent2D GetFramebufferExtent();

        /**
         * Obtains a list and count of required extensions needed by the platform layer.
         * TODO: This is Vulkan-specific, and should be abstracted somehow later on.
         *
         * @param extensionCount A pointer to a number to which the number of extensions will be assigned.
         * @param extensionNames A pointer to an array to which the names of required extensions will be assigned.
         */
        void GetRequiredExtensions( U32* extensionCount, const char*** extensionNames );

        /**
         * Creates a surface to be used by the renderer
         * TODO: This is Vulkan-specific, and should be abstracted somehow later on.
         *
         * @param instance A pointer to the Vulkan instance.
         * @param VkSurfaceKHR A pointer to an Vulkan surface to which the created surface will be assigned.
         */
        void CreateSurface( VkInstance instance, VkSurfaceKHR* surface );

        /**
         * Begins the main game loop.
         *
         * @returns True on success; false on failure. Returning false crashes the application.
         */
        const bool StartGameLoop();

        /**
         * Waits for any asynchronous events to finish before returning. Used for synchronization purposes.
         */
        void WaitEvents();
    private:
        static void onFrameBufferResize( GLFWwindow* window, I32 width, I32 height );
    private:
        Engine* _engine;
        GLFWwindow* _window;
        Clock* _clock;
        U64 _lastTime;

        friend class TGLFWWindow;
    };
}
