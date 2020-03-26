
#include <GLFW/glfw3.h>

#include "TGLFWWindow.h"
#include "Platform.h"

namespace Epoch {

    TGLFWWindow::TGLFWWindow( const char* windowName, const U32 width, const U32 height ) {
        _handle = glfwCreateWindow( width, height, windowName, nullptr, nullptr );
        glfwSetWindowUserPointer( _handle, this );
        glfwSetFramebufferSizeCallback( _handle, Platform::onFrameBufferResize );
    }

    const Extent2D TGLFWWindow::GetFramebufferExtent() const {
        Extent2D extents;
        glfwGetFramebufferSize( _handle, &extents.Width, &extents.Height );
        return extents;
    }

    void* TGLFWWindow::GetHandle() {
        return _handle;
    }
}