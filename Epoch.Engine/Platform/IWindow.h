#pragma once

#include <string>
#include "../Types.h"

namespace Epoch {

    struct WindowCreateInfo {
        F32 StartPositionX;
        F32 StartPositionY;
        F32 StartWidth;
        F32 StartHeight;

        bool AcceptsInput;

        /*
         * Indicates if this window is a standard window with borders, etc. or if it is a special-case window, such as a tooltip.
         */
        bool IsStandardWindow;

        /**
         * Indicates if the OS window border will be used, or a custom one created by the engine.
         */
        bool HasOSWindowBorder;
        bool IsTopmostWindow;
        bool HasCloseButton;
        bool HasMinimize;
        bool HasMaximize;
        bool HasSizingFrame;

        std::string Title;
    };

    class IApplication;

    class IWindow {
    public:

        /**
         * Returns the extents (visible size in pixels) of the framebuffer.
         *
         * @returns A the framebuffer extents.
         */
        virtual const Extent2D GetFramebufferExtent() const = 0;

        virtual void* GetHandle() = 0;

        virtual void Initialize( IApplication* application, const WindowCreateInfo& definition, const bool showImmediate ) = 0;
    };
}