
#include "../../Engine.h"
#include "../../Logger.h"
#include "../Backend/IRendererBackend.h"
#include "../Backend/Vulkan/VulkanRenderer.h"
#include "../TextureCache.h"
#include "../Material.h"

#include "RendererFrontend.h"

// TEMP
#include "../../Assets/StaticMesh/StaticMesh.h"
#include "../../Resources/ITexture.h"

namespace Epoch {

    // A pointer to the engine which owns this renderer.
    Engine* RendererFrontEnd::_engine;

    // A pointer to the back end.
    IRendererBackend* RendererFrontEnd::_backend;

    // The internal texture cache.
    TextureCache* RendererFrontEnd::_textureCache;

    // TEMP
    static StaticMesh* testMesh;
    static ITexture* textures[2];
    static U64 updatesTemp = 0;
    static U64 currentTextureIndex = 0;

    const bool RendererFrontEnd::Initialize( Engine* engine ) {

        _engine = engine;
        Logger::Log( "Created renderer front end." );

        // TODO: Choose this from configuration instead of hardcoding it.
        // TODO: Should probably be created via factory to prevent this class from knowing about the specific type.
        _backend = new VulkanRenderer( _engine->GetPlatform() );
        _backend->Initialize();

        _textureCache = new TextureCache();
        MaterialManager::Initialize();

        // TEMP: Load a test mesh
        testMesh = StaticMesh::FromFile( "test", "assets/models/sibenik.obj", true );
        textures[0] = _backend->GetTexture( TString( "test512" ), TString( "assets/textures/test512.png" ) );
        textures[1] = _backend->GetTexture( TString( "testice" ), TString( "assets/textures/testice.jpg" ) );

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

        // Update descriptors if need be. TEST: Swap texture
        // Must happen before queue is started below.
        updatesTemp++;
        if( updatesTemp > 3000 ) {
            currentTextureIndex = ( currentTextureIndex == 0 ? 1 : 0 );
            updatesTemp = 0;
        }

        return _backend->Frame( deltaTime );
    }

    const bool RendererFrontEnd::UploadMeshData( const MeshUploadData& data, MeshRendererReferenceData* referenceData ) {
        return _backend->UploadMeshData( data, referenceData );
    }

    void RendererFrontEnd::FreeMeshData( const U64 index ) {
        _backend->FreeMeshData( index );
    }

    ITexture* RendererFrontEnd::GetTexture( const TString& name, const TString& path, const bool bypassCache ) {
        ITexture* texture;
        if( !bypassCache && _textureCache->Exists( name ) ) {
            _textureCache->GetTextureReference( name, &texture );
        } else {
            texture = _backend->GetTexture( name, path );
        }

        return texture;
    }

    ITexture* RendererFrontEnd::GetDefaultWhiteTexture() {
        return _textureCache->GetDefaultWhiteTexture();
    }

    void RendererFrontEnd::ReleaseTexture( const TString& name ) {
        if( _textureCache->Exists( name ) ) {
            _textureCache->Release( name );
        } else {
            Logger::Warn( "Tried to release a texture not owned by the renderer." );
        }
    }

    IShader* RendererFrontEnd::GetBuiltinMaterialShader( const MaterialType type ) {
        return _backend->GetBuiltinMaterialShader( type );
    }
}