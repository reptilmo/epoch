
#include "Renderer/Frontend/RendererFrontend.h"

#include "Logger.h"
#include "Events/EventManager.h"
#include "Time/Clock.h"
#include "World/World.h"

#include "Engine.h"

namespace Epoch {

    Engine::Engine( IApplication* application ) {
        Epoch::Logger::Log( "Initializing Epoch Engine: %d", 4 );
        _application = application;     
    }

    Engine::~Engine() {
        RendererFrontEnd::Shutdown();

        if( _world ) {
            delete _world;
            _world = nullptr;
        }

        _application = nullptr;
    }

    void Engine::Run() {
        if( !RendererFrontEnd::Initialize( this ) ) {
            Logger::Fatal( "Failed to initialize renderer!" );
        }

        // Create the world.
        _world = new World();
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