
#include "Logger.h"
#include "Platform.h"
#include "VulkanRenderer.h"
#include "Engine.h"

namespace Epoch {

    Engine::Engine( const char* applicationName ) {
        Epoch::Logger::Log( "Initializing Epoch Engine: %d", 4 );
        _platform = new Platform( this, applicationName );
        _renderer = new VulkanRenderer( _platform );
    }

    Engine::~Engine() {

    }

    void Engine::Run() {
        _platform->StartGameLoop();
    }

    void Engine::OnLoop( const F32 deltaTime ) {
        _renderer->Frame( deltaTime );
    }
}