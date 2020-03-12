
#include "Renderer/Frontend/RendererFrontend.h"

#include "Logger.h"
#include "Platform.h"

#include "Engine.h"

namespace Epoch {

    Engine::Engine( const char* applicationName ) {
        Epoch::Logger::Log( "Initializing Epoch Engine: %d", 4 );
        _platform = new Platform( this, applicationName );
        _renderer = new RendererFrontEnd( this );
    }

    Engine::~Engine() {
        if( _renderer ) {
            delete _renderer;
            _renderer = nullptr;
        }

        if( _platform ) {
            delete _platform;
            _platform = nullptr;
        }
    }

    void Engine::Run() {
        if( !_renderer->Initialize() ) {
            Logger::Fatal( "Failed to initialize renderer!" );
        }

        if( _platform ) {
            _platform->StartGameLoop();
        }
    }

    void Engine::OnLoop( const F32 deltaTime ) {
        if( _renderer ) {
            _renderer->Frame( deltaTime );
        }
    }
}