#pragma once


#include "../IWindow.h"

namespace Epoch {

    class IApplication;

    /**
     *A Win32-specific window implementation of IWindow.
     */
    class WindowsWindow final : public IWindow {
    public:
        static const wchar_t* ApplicationWindowClass;
    public:
        static WindowsWindow* Create();

    public:
        virtual const Extent2D GetFramebufferExtent() const;

        void* GetHandle() { return _handle; }

        ~WindowsWindow();
        void Initialize( IApplication* application, const WindowCreateInfo createInfo ) override;
        void RequestClose() override;
        void Show() override;
        void Hide() override;
        const bool CloseRequested() const override { return _closeRequested; }
    private:
        WindowsWindow();

    private:
        bool _isVisible = false;
        bool _closeRequested = false;
        WindowCreateInfo _createInfo;
        HWND _handle;
    };
}