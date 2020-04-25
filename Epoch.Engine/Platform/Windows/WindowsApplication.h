#pragma once

#include "../IApplication.h"
#include "../../String/TString.h"

namespace Epoch {

    class Engine;
    class IWindow;
    class WindowsWindow;
    class Clock;

    /*
     A Win32-specific implementation of IApplication.
    */
    class WindowsApplication final : public IApplication {
    public:
        static WindowsApplication* CreateWindowsApplication( const HINSTANCE handle, const HICON iconHandle );

        ~WindowsApplication();
    public:
        void Run() override;
        const bool Initialize();
        void PumpMessages( const F32 deltaTime );
        IWindow* CreateApplicationWindow( const WindowCreateInfo& createInfo );
        I32 ProcessMessage( HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam );

        virtual IWindow* GetApplicationWindow() override;

    private:
        WindowsApplication( const HINSTANCE handle, const HICON iconHandle );

        const bool registerClass( const HINSTANCE handle, const HICON iconHandle );
    private:
        TString _applicationName;
        Engine* _engine;
        HINSTANCE _handle;
        WindowsWindow* _mainWindow;
        Clock* _clock;
        U64 _lastTime;
    };
}