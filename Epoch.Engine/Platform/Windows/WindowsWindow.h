#pragma once


#include "../IWindow.h"

namespace Epoch {

    class IApplication;

    class WindowsWindow final : public IWindow {
    public:
        static const TCHAR ApplicationWindowClass[];
    public:
        static WindowsWindow* Create();

    public:
        virtual const Extent2D GetFramebufferExtent() const { return Extent2D{ 0, 0 }; };

        void* GetHandle() { return _handle; }

        ~WindowsWindow();
        void Initialize( IApplication* application, const WindowCreateInfo& createInfo, const bool showImmediate ) override;

    private:
        WindowsWindow();

    private:
        HWND _handle;
    };
}