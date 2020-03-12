#pragma once

#include "Types.h"

namespace Epoch {

    class Platform;
    class RendererFrontEnd;

    class Engine {
    public:
        Engine( const char* applicationName );
        ~Engine();

        void Run();

        Platform* GetPlatform() { return _platform; }

        void OnLoop( const F32 deltaTime );

    private:
        Platform* _platform;
        RendererFrontEnd* _renderer;
    };
}