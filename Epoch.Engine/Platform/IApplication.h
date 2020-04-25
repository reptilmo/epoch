#pragma once

#include "../Defines.h"
#include "../Types.h"
#include "../Events/Event.h"

namespace Epoch {

    struct WindowCreateInfo;

    class IWindow;

    /**
     * A specialized event used to signal a window resize having occurred.
     */
    struct WindowResizedEvent final : public Event {

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

        WindowResizedEvent( void* sender, U32 prevWidth, U32 prevHeight, U32 newWidth, U32 newHeight )
            : Event( EventType::WINDOW_RESIZED, sender ) {

            PreviousWidth = prevWidth;
            PreviousHeight = prevHeight;
            NewWidth = newWidth;
            NewHeight = newHeight;
        }

    };

    /**
     * An interface for the application layer. The application handles tasks such as window creation, timing, resource
     * access such as CPU/Memory, and other operating-specific details which can be abstracted. The application layer also
     * handles the creation of the Engine object and ultimately owns it. This also owns the main game loop.
     */
    class EPOCH_API IApplication {
    public:

        /**
         * Runs this application.
         */
        virtual void Run() = 0;

        /**
         * Performs initialization routines on this application.
         *
         * @returns True on success; otherwise false.
         */
        virtual const bool Initialize() = 0;

        /**
         * Performs platform-specific message pumping. Typically called internally by the application.
         */
        virtual void PumpMessages( const F32 deltaTime ) = 0;

        /**
         * Creates a new application window using the provided creation info.
         *
         * @param createInfo The settings used to create the new window.
         *
         * @returns The newly-created application window.
         */
        virtual IWindow* CreateApplicationWindow( const WindowCreateInfo& createInfo ) = 0;

        /**
         * Obtains a pointer to the main application window.
         */
        virtual IWindow* GetApplicationWindow() = 0;
    };

    /**
     * A singleton application factory.
     */
    class EPOCH_API Application final {
    public:

        /**
         * Creates a platform-specific application for the current target platform.
         *
         * @param applicationName The name of the application. Typically also used for the main window title.
         *
         * @returns A pointer to the newly-created application.
         */
        static IApplication* CreateApplication( const wchar_t* applicationName );

    private:
        Application() {}
        ~Application() {}
    };
}