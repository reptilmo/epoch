#pragma once

#include "../IApplication.h"

namespace Epoch {

    class Engine;
    class WindowsWindow;

    class WindowsApplication final : public IApplication {    
    public:
        static WindowsApplication* CreateWindowsApplication( const HINSTANCE handle, const HICON iconHandle );

        ~WindowsApplication();
    public:
        void Run() override;
        const bool Initialize( Engine* engine );
        void PumpMessages( const F32 deltaTime );
        IWindow* CreateApplicationWindow( const WindowCreateInfo& createInfo, const bool showImmediate );
        I32 ProcessMessage( HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam );

    private:
        WindowsApplication( const HINSTANCE handle, const HICON iconHandle );
        
        const bool registerClass( const HINSTANCE handle, const HICON iconHandle );
    private:
        Engine* _engine;
        HINSTANCE _handle;
        WindowsWindow* _mainWindow;
    };
}