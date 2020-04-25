#pragma once

#include "../String/TString.h"
#include "../Types.h"

namespace Epoch {

    /**
     * A structure used for the creation of windows.
     */
    struct WindowCreateInfo {

        /**
         * The X-position this window will be created at.
         */
        F32 StartPositionX;

        /**
         * The Y-position this window will be created at.
         */
        F32 StartPositionY;

        /**
         * The width this window will be created at.
         */
        F32 StartWidth;

        /**
         * The height this window will be created at.
         */
        F32 StartHeight;

        /**
         * Indicates if this window accepts keyboard and mouse input.
         */
        bool AcceptsInput;

        /*
         * Indicates if this window is a standard window with borders, etc. or if it is a special-case window, such as a tooltip.
         */
        bool IsStandardWindow;

        /**
         * Indicates if the OS window border will be used, or a custom one created by the engine.
         */
        bool HasOSWindowBorder;

        /**
         * Indicates if this window should remain topmost over all other windows.
         */
        bool IsTopmostWindow;

        /**
         * Indicates if this window has a close button.
         */
        bool HasCloseButton;

        /**
         * Indicates if this window has a minimize button.
         */
        bool HasMinimize;

        /**
         * Indicates if this window has a maximize button.
         */
        bool HasMaximize;

        /**
         * Indicates if this window has a frame to resize.
         */
        bool HasSizingFrame;

        /**
         * The title to be used by the window in the title bar.
         */
        const wchar_t* Title;
    };

    class IApplication;

    /**
     * An interface for an application window. Can be used for anything from the main application itself
     * to tool or editor windows.
     */
    class IWindow {
    public:

        /**
         * Returns the extents (visible size in pixels) of the framebuffer.
         *
         * @returns A the framebuffer extents.
         */
        virtual const Extent2D GetFramebufferExtent() const = 0;

        /**
         * Returns the internal handle of this window.
         */
        virtual void* GetHandle() = 0;

        /**
         * Performs initialization routines on this window.
         * 
         * @param application A pointer to the application that owns this window.
         * @param definition A collection of settings used for the creation of this window.
         */
        virtual void Initialize( IApplication* application, const WindowCreateInfo definition ) = 0;

        /**
         * Requests that this window be closed. The actual close is handled at the application level.
         * Typically triggered by clicking the close button on the actual window.
         */
        virtual void RequestClose() = 0;

        /**
         * Shows this window.
         */
        virtual void Show() = 0;

        /**
         * Hides this window.
         */
        virtual void Hide() = 0;

        /**
         * Indicates if this window has been requested to be closed.
         */
        virtual const bool CloseRequested() const = 0;
    };
}