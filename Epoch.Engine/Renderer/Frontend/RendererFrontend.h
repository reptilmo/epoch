#pragma once

#include "../../Types.h"

namespace Epoch {

    class Engine;
    class IRendererBackend;

    class RendererFrontEnd final {
    public:
        RendererFrontEnd( Engine* engine );
        ~RendererFrontEnd();

        const bool Initialize();

        const bool Frame( const F32 deltaTime );

    private:
        Engine* _engine;
        IRendererBackend* _backend;
    };
}