#pragma once

#include "../Types.h"

namespace Epoch {

    struct WindowCreateInfo;

    class Engine;
    class IWindow;

    class IApplication {
    public:
        virtual void Run() = 0;
        virtual const bool Initialize( Engine* engine ) = 0;
        virtual void PumpMessages( const F32 deltaTime ) = 0;
        virtual IWindow* CreateApplicationWindow( const WindowCreateInfo& createInfo, const bool showImmediate ) = 0;
    };

    class Application {
    public:
        static IApplication* CreateApplication( const char* applicationName );
    };
}