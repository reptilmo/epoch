
#include <Windows.h>

#include "../../Types.h"
#include "../../Engine.h"
#include "../../Memory/Memory.h"

#include "WindowsWindow.h"
#include "WindowsApplication.h"


namespace Epoch {

    WindowsApplication* App;

    LRESULT CALLBACK ApplicationWindowProcedure( HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam );

    void WindowsApplication::Run() {

    }

    WindowsApplication* WindowsApplication::CreateWindowsApplication( const HINSTANCE handle, const HICON iconHandle ) {
        App = new WindowsApplication( handle, iconHandle );
        return App;
    }

    const bool WindowsApplication::Initialize( Engine* engine ) {
        _engine = engine;

        return true;
    }

    void WindowsApplication::PumpMessages( const F32 deltaTime ) {

        MSG message;

        while( PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) ) {
            TranslateMessage( &message );
            DispatchMessage( &message );
        }
    }

    IWindow* WindowsApplication::CreateApplicationWindow( const WindowCreateInfo& createInfo, const bool showImmediate ) {
        _mainWindow = WindowsWindow::Create();
        _mainWindow->Initialize( this, createInfo, showImmediate );
        return _mainWindow;
    }

    WindowsApplication::WindowsApplication( const HINSTANCE handle, const HICON iconHandle ) {
        _handle = handle;

        // Register the win32 window class.
        const bool classRegistered = registerClass( handle, iconHandle );

        // TODO: setup input, etc.
    }

    WindowsApplication::~WindowsApplication() {

    }

    I32 WindowsApplication::ProcessMessage( HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam ) {
        // TODO: get the appropriate window by its handle

        switch( msg ) {
        case WM_ERASEBKGND:
            // Notify the OS that erasing will be handled by the application to prevent flicker.
            return 1;
        case WM_DESTROY:

            return 0;
        }

        return 0;
    }

    const bool WindowsApplication::registerClass( const HINSTANCE handle, const HICON iconHandle ) {
        WNDCLASS wc;
        TMemory::MemZero( &wc, sizeof( wc ) );
        wc.style = CS_DBLCLKS; // Get double-clicks
        wc.lpfnWndProc = ApplicationWindowProcedure;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = handle;
        wc.hIcon = iconHandle;
        wc.hCursor = NULL; // Manage the cursor manually
        wc.hbrBackground = NULL;// Transparent
        wc.lpszClassName = WindowsWindow::ApplicationWindowClass;

        if( !RegisterClass( &wc ) ) {
            MessageBox( nullptr, L"Window registration failed", L"Error", MB_ICONEXCLAMATION | MB_OK );
            return false;
        }

        return true;
    }

    LRESULT CALLBACK ApplicationWindowProcedure( HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam ) {
        return App->ProcessMessage( hwnd, msg, wParam, lParam );
    }

    IApplication* Application::CreateApplication( const char* applicationName ) {
        HICON icon = LoadIcon( (HINSTANCE)NULL, IDI_APPLICATION );
        return WindowsApplication::CreateWindowsApplication( GetModuleHandle( 0 ), icon );
    }
}