
#include "Renderer/Frontend/RendererFrontend.h"

#include "Logger.h"
#include "Platform/Platform.h"
#include "Events/EventManager.h"
#include "Time/Clock.h"

#include "Engine.h"

namespace Epoch {

    Engine::Engine( const char* applicationName ) {
        Epoch::Logger::Log( "Initializing Epoch Engine: %d", 4 );
        _platform = new Platform( this, applicationName );
    }

    Engine::~Engine() {
        RendererFrontEnd::Shutdown();

        if( _platform ) {
            delete _platform;
            _platform = nullptr;
        }
    }

    void Engine::Run() {
        if( !RendererFrontEnd::Initialize( this ) ) {
            Logger::Fatal( "Failed to initialize renderer!" );
        }

        if( _platform ) {
            _platform->StartGameLoop();
        }
    }

    const bool Engine::OnLoop( const F32 deltaTime ) {
        EventManager::Update( deltaTime );

        if( !RendererFrontEnd::Frame( deltaTime ) ) {
            Logger::Error( "Renderer failed! See logs for details." );
            return false;
        }

        return true;
    }
}