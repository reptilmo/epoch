
#include <Windows.h>

#include "../../Logger.h"
#include "../../Math/TMath.h"

#include "WindowsApplication.h"
#include "WindowsWindow.h"



namespace Epoch {

    const wchar_t* WindowsWindow::ApplicationWindowClass = L"EpochWindow";

    WindowsWindow* WindowsWindow::Create() {
        return new WindowsWindow();
    }

    WindowsWindow::WindowsWindow() {

    }

    WindowsWindow::~WindowsWindow() {

    }

    const Extent2D WindowsWindow::GetFramebufferExtent() const {

        RECT rect;
        GetWindowRect( _handle, &rect );
        I32 width = rect.right - rect.left;
        I32 height = rect.bottom - rect.top;
        return Extent2D{ width, height };
    }

    void WindowsWindow::Initialize( IApplication* application, const WindowCreateInfo createInfo ) {
        _createInfo = createInfo;
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
        _handle = CreateWindowExW( windowExStyle, ApplicationWindowClass, createInfo.Title,
            windowStyle, windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, GetModuleHandleW( 0 ), nullptr ); // TEMP hinstance

        if( _handle == nullptr ) {
            MessageBoxW( NULL, TEXT( "Window creation failed!" ), TEXT( "Error!" ), MB_ICONEXCLAMATION | MB_OK );
            Logger::Fatal( "Window creation failed!" );
        }
    }

    void WindowsWindow::RequestClose() {
        _closeRequested = true;
    }

    void WindowsWindow::Show() {
        if( !_isVisible ) {

            bool shouldActivate = false;
            if( _createInfo.AcceptsInput ) {
                // TODO: add more complex logic to handle if this is the first time shown.
                shouldActivate = true;
            }

            int showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;
            // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
            // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE

            ::ShowWindow( _handle, showWindowCommandFlags );

            _isVisible = true;
        }
    }

    void WindowsWindow::Hide() {
        if( _isVisible ) {


            _isVisible = false;
        }
    }
}