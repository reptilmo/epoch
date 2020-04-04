
#include "../../Engine.h"
#include "../../Logger.h"
#include "../Backend/IRendererBackend.h"
#include "../Backend/Vulkan/VulkanRenderer.h"
#include "RendererFrontend.h"

// TEMP
#include "../../Assets/StaticMesh/StaticMesh.h"

namespace Epoch {

    // A pointer to the engine which owns this renderer.
    Engine* RendererFrontEnd::_engine;

    // A pointer to the back end.
    IRendererBackend* RendererFrontEnd::_backend;

    // TEMP
    static StaticMesh* testMesh;

    const bool RendererFrontEnd::Initialize( Engine* engine ) {

        _engine = engine;
        Logger::Log( "Created renderer front end." );

        // TODO: Choose this from configuration instead of hardcoding it.
        // TODO: Should probably be created via factory to prevent this class from knowing about the specific type.
        _backend = new VulkanRenderer( _engine->GetPlatform() );
        _backend->Initialize();

        // TEMP: Load a test mesh
        testMesh = StaticMesh::FromFile( "test", "assets/models/sibenik.obj", true );

        return true;
    }

    void RendererFrontEnd::Shutdown() {
        _engine = nullptr;
        if( _backend ) {
            _backend->Destroy();
            delete _backend;
            _backend = nullptr;
        }
    }

    const bool RendererFrontEnd::Frame( const F32 deltaTime ) {

        // TODO: Get objects from scenegraph.
        if( testMesh->IsUploaded() ) {
            U64 submeshCount = testMesh->GetMeshDataCount();
            for( U64 i = 0; i < submeshCount; ++i ) {
                _backend->AddToFrameRenderList( testMesh->GetMeshReferenceData( i ) );
            }
        }

        // TODO: All front-end work goes here (scene sorting, culling, etc) before the frame call.

        // If the frame preparation indicates we should wait, boot out early.
        if( !_backend->PrepareFrame( deltaTime ) ) {
            return true;
        }

        return _backend->Frame( deltaTime );
    }

    const bool RendererFrontEnd::UploadMeshData( const MeshUploadData& data, MeshRendererReferenceData* referenceData ) {
        return _backend->UploadMeshData( data, referenceData );
    }

    void RendererFrontEnd::FreeMeshData( const U64 index ) {
        _backend->FreeMeshData( index );
    }
}