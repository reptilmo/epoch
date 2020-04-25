
#include <Windows.h>

#include "../../Types.h"
#include "../../Engine.h"
#include "../../Memory/Memory.h"
#include "../../Time/Clock.h"
#include "../../Logger.h"

#include "WindowsWindow.h"
#include "WindowsApplication.h"


namespace Epoch {

    WindowsApplication* App;

    LRESULT CALLBACK ApplicationWindowProcedure( HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam );

    void WindowsApplication::Run() {
        _clock = new Clock( true );
        if( _engine ) {
            _engine->Run();
        }

        _lastTime = _clock->GetTime();

        // MAIN GAME LOOP
        while( !_mainWindow->CloseRequested() ) {

            U64 currentTime = _clock->GetTime();
            F32 delta = (F32)( (F64)currentTime - (F64)_lastTime );
            delta *= 0.001f; // Convert to seconds.

            PumpMessages( delta );

            if( !_mainWindow->CloseRequested() ) {
                if( !_engine->OnLoop( delta ) ) {
                    Logger::Fatal( "Engine loop failed! See logs for details." );
                }

                _lastTime = currentTime;
            }
        }

        // Post a message to actually destroy the window.
        PostMessageW( (HWND)_mainWindow->GetHandle(), WM_DESTROY, 0, 0 );
    }

    WindowsApplication* WindowsApplication::CreateWindowsApplication( const HINSTANCE handle, const HICON iconHandle ) {
        App = new WindowsApplication( handle, iconHandle );

        return App;
    }

    const bool WindowsApplication::Initialize() {
        _engine = new Engine( this );

        return true;
    }

    void WindowsApplication::PumpMessages( const F32 deltaTime ) {

        MSG message;

        while( PeekMessageW( &message, NULL, 0, 0, PM_REMOVE ) ) {
            TranslateMessage( &message );
            DispatchMessageW( &message );
        }
    }

    IWindow* WindowsApplication::CreateApplicationWindow( const WindowCreateInfo& createInfo ) {
        _mainWindow = WindowsWindow::Create();
        _mainWindow->Initialize( this, createInfo );
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
        case WM_CLOSE:
            _mainWindow->RequestClose();
            return 0;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
        case WM_SIZING: {
            // While resizing

            break;
        }
        case WM_SIZE: {
            // After resize is complete
            U16 width = LOWORD( lParam );
            U16 height = HIWORD( lParam );

            // TODO: Detect which window was resized.
            WindowResizedEvent* resizeEvent = new WindowResizedEvent( _mainWindow, 0, 0, (U32)width, (U32)height );
            resizeEvent->Post( true );
            break;
        }

        case WM_KEYDOWN:
            // TODO: put keyboard input hooks here.
            switch( wParam ) {
            case VK_ESCAPE:
                _mainWindow->RequestClose();
                break;
            }
            break;
        }

        return DefWindowProcW( hwnd, msg, wParam, lParam );
    }

    IWindow* WindowsApplication::GetApplicationWindow() {
        return static_cast<IWindow*>( _mainWindow );
    }

    const bool WindowsApplication::registerClass( const HINSTANCE handle, const HICON iconHandle ) {
        WNDCLASSW wc;
        TMemory::MemZero( &wc, sizeof( wc ) );
        wc.style = CS_DBLCLKS; // Get double-clicks
        wc.lpfnWndProc = ApplicationWindowProcedure;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = handle;
        wc.hIcon = iconHandle;
        wc.hCursor = LoadCursor( NULL, IDC_ARROW );// NULL; // Manage the cursor manually
        wc.hbrBackground = NULL;// Transparent
        wc.lpszClassName = WindowsWindow::ApplicationWindowClass;

        if( !RegisterClassW( &wc ) ) {
            MessageBoxW( nullptr, L"Window registration failed", L"Error", MB_ICONEXCLAMATION | MB_OK );
            return false;
        }

        return true;
    }

    LRESULT CALLBACK ApplicationWindowProcedure( HWND hwnd, U32 msg, WPARAM wParam, LPARAM lParam ) {
        return App->ProcessMessage( hwnd, msg, wParam, lParam );
    }

    IApplication* Application::CreateApplication( const wchar_t* applicationName ) {
        HICON icon = LoadIcon( (HINSTANCE)NULL, IDI_APPLICATION );
        WindowsApplication* app = WindowsApplication::CreateWindowsApplication( GetModuleHandleW( 0 ), icon );
        app->Initialize();

        // TODO: Load this from config if it exists.
        WindowCreateInfo createInfo;
        createInfo.IsStandardWindow = true;
        createInfo.AcceptsInput = true;
        createInfo.HasCloseButton = true;
        createInfo.HasMaximize = true;
        createInfo.HasMinimize = true;
        createInfo.HasSizingFrame = true;
        createInfo.HasOSWindowBorder = true;
        createInfo.StartHeight = 720.0f;
        createInfo.StartWidth = 1280.0f;
        createInfo.StartPositionX = 100.0f;
        createInfo.StartPositionY = 100.0f;
        createInfo.Title = applicationName;
        IWindow* window = app->CreateApplicationWindow( createInfo );

        window->Show();

        return app;
    }
}