#pragma once

#include "IWindow.h"

struct GLFWwindow;

namespace Epoch {

    class IApplication;
    struct WindowCreateInfo;

    class TGLFWWindow final : public IWindow {
    public:

        TGLFWWindow( const char* windowName, const U32 width, const U32 height );

        /**
         * Returns the extents (visible size in pixels) of the framebuffer.
         *
         * @returns A the framebuffer extents.
         */
        const Extent2D GetFramebufferExtent() const override;

        void* GetHandle() override;

        // NOTE: This is here as a stop-gap until GLFW is removed and replaced.
        void Initialize( IApplication* application, const WindowCreateInfo& definition, const bool showImmediate ) {}
    private:
        GLFWwindow* _handle;
    };
}