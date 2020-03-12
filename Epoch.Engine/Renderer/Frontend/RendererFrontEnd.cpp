
#include "../../Engine.h"
#include "../../Logger.h"
#include "../Backend/IRendererBackend.h"
#include "../Backend/Vulkan/VulkanRenderer.h"
#include "RendererFrontend.h"

namespace Epoch {

    RendererFrontEnd::RendererFrontEnd( Engine* engine ) {
        _engine = engine;
        Logger::Log( "Created renderer front end." );
    }

    RendererFrontEnd::~RendererFrontEnd() {
        _engine = nullptr;
        if( _backend ) {
            delete _backend;
            _backend = nullptr;
        }
    }

    const bool RendererFrontEnd::Initialize() {

        // TODO: Choose this from configuration instead of hardcoding it.
        // TODO: Should probably be created via factory to prevent this class from knowing about the specific type.
        _backend = new VulkanRenderer( _engine->GetPlatform() );

        return true;
    }

    const bool RendererFrontEnd::Frame( const F32 deltaTime ) {

        // TODO: All front-end work goes here (scene sorting, culling, etc) before the frame call.

        return _backend->Frame( deltaTime );
    }
}