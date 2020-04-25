
#include "Renderer/Frontend/RendererFrontend.h"

#include "Logger.h"
#include "Platform/Platform.h"
#include "Events/EventManager.h"
#include "Time/Clock.h"
#include "World/World.h"

#include "Engine.h"

namespace Epoch {

    Engine::Engine( const char* applicationName ) {
        Epoch::Logger::Log( "Initializing Epoch Engine: %d", 4 );
        _platform = new Platform( this, applicationName );        
    }

    Engine::~Engine() {
        RendererFrontEnd::Shutdown();

        if( _world ) {
            delete _world;
            _world = nullptr;
        }

        if( _platform ) {
            delete _platform;
            _platform = nullptr;
        }
    }

    void Engine::Run() {
        if( !RendererFrontEnd::Initialize( this ) ) {
            Logger::Fatal( "Failed to initialize renderer!" );
        }

        // Create the world.
        _world = new World();

        if( _platform ) {
            _platform->StartGameLoop();
        }
    }

    const bool Engine::OnLoop( const F32 deltaTime ) {
        EventManager::Update( deltaTime );

        _world->Update( deltaTime );

        if( !RendererFrontEnd::Frame( _world, deltaTime ) ) {
            Logger::Error( "Renderer failed! See logs for details." );
            return false;
        }

        return true;
    }
}