
#include <Windows.h>

#include "../../Logger.h"
#include "../../Math/TMath.h"

#include "WindowsApplication.h"
#include "WindowsWindow.h"

namespace Epoch {

    const TCHAR WindowsWindow::ApplicationWindowClass[] = L"EpochWindow";

    WindowsWindow* WindowsWindow::Create() {
        return new WindowsWindow();
    }

    WindowsWindow::WindowsWindow() {

    }

    WindowsWindow::~WindowsWindow() {

    }

    void WindowsWindow::Initialize( IApplication* application, const WindowCreateInfo& createInfo, const bool showImmediate ) {
        U32 windowExStyle = 0;
        U32 windowStyle = 0;

        U32 clientX = TMath::Ftoi32( createInfo.StartPositionX );
        U32 clientY = TMath::Ftoi32( createInfo.StartPositionY );
        U32 clientWidth = TMath::Ftoi32( createInfo.StartWidth );
        U32 clientHeight = TMath::Ftoi32( createInfo.StartHeight );

        U32 windowX = clientX;
        U32 windowY = clientY;
        U32 windowWidth = clientWidth;
        U32 windowHeight = clientHeight;

        if( createInfo.HasOSWindowBorder ) {
            // OS window border setup
            windowExStyle = WS_EX_APPWINDOW;
            windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;

            if( createInfo.IsStandardWindow ) {
                if( createInfo.HasMaximize ) {
                    windowStyle |= WS_MAXIMIZEBOX;
                }

                if( createInfo.HasMinimize ) {
                    windowStyle |= WS_MINIMIZEBOX;
                }

                if( createInfo.HasSizingFrame ) {
                    windowStyle |= WS_THICKFRAME;
                } else {
                    windowStyle += WS_BORDER;
                }
            } else {
                windowStyle |= WS_POPUP | WS_BORDER;
            }

            // Get the size of the border.
            RECT borderRect = { 0, 0, 0, 0 };
            AdjustWindowRectEx( &borderRect, windowStyle, false, windowExStyle );

            // Border rectangle in this case is negative.
            windowX += borderRect.left;
            windowY += borderRect.top;

            // Grow the window size by the OS border. This makes the client width/height correct.
            windowWidth += borderRect.right - borderRect.left;
            windowHeight += borderRect.bottom - borderRect.top;
        } else {
            // TODO: support custom window borders.
            Logger::Fatal( "Non-OS border configuration not yet supported." );
        }

        // Create the actual window
        /*_handle = CreateWindowEx( windowExStyle, ApplicationWindowClass, (LPCWSTR)createInfo.Title.c_str(),
            windowStyle, windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr );*/

        _handle = CreateWindowEx( windowExStyle, ApplicationWindowClass, (LPCWSTR)createInfo.Title.c_str(),
            windowStyle, windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, GetModuleHandle( 0 ), nullptr ); // TEMP hinstance

        if( _handle == nullptr ) {
            MessageBox( NULL, TEXT( "Window creation failed!" ), TEXT( "Error!" ), MB_ICONEXCLAMATION | MB_OK );
            Logger::Fatal( "Window creation failed!" );
        }

        //U32 setWindowPosFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;
        //SetWindowPos( _handle, nullptr, 0, 0, 0, 0, setWindowPosFlags );

    }
}