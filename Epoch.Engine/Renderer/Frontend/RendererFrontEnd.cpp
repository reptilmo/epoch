
#include "../../Resources/ITexture.h"
#include "../../Engine.h"
#include "../../Logger.h"
#include "../Backend/IRendererBackend.h"
#include "../Backend/Vulkan/VulkanRendererBackend.h"
#include "../TextureCache.h"
#include "../Material.h"
#include "../../World/World.h"

#include "RendererFrontend.h"

namespace Epoch {

    // A pointer to the engine which owns this renderer.
    Engine* RendererFrontEnd::_engine;

    // A pointer to the back end.
    IRendererBackend* RendererFrontEnd::_backend;

    // The internal texture cache.
    TextureCache* RendererFrontEnd::_textureCache;

    const bool RendererFrontEnd::Initialize( Engine* engine ) {

        _engine = engine;
        const bool validationEnabled = false;
        Logger::Log( "Created renderer front end. Renderer validation %s enabled.", validationEnabled ? "IS" : "IS NOT" );

        // TODO: Choose this from configuration instead of hardcoding it.
        // TODO: Should probably be created via factory to prevent this class from knowing about the specific type.
        _backend = new VulkanRendererBackend( _engine->GetApplication() );
        _backend->Initialize( validationEnabled );

        _textureCache = new TextureCache();
        _textureCache->Initialize();
        MaterialManager::Initialize();

        return true;
    }

    void RendererFrontEnd::Shutdown() {
        _engine = nullptr;

        if( _backend ) {
            _backend->Shutdown();
        }

        MaterialManager::Shutdown();

        if( _textureCache ) {
            delete _textureCache;
            _textureCache = nullptr;
        }

        if( _backend ) {
            _backend->Destroy();
            delete _backend;
            _backend = nullptr;
        }
    }

    const bool RendererFrontEnd::Frame( World* world, const F32 deltaTime ) {

        // TODO: Get a pointer to the world, then  the ask it for all objects to be rendered.
        // Sort these by entity type. Render sky, then statics, terrain, animated, then special items such as fog, water, etc. 
        WorldRenderableObjectTable* objectTable = world->GetRenderableObjects();

        // TODO: All front-end work goes here (scene sorting, culling, etc) before adding the object to the render table.

        /*if( objectTable->StaticMeshCount > 1 ) {
            sortByMaterialShader( objectTable->StaticMeshes, objectTable->StaticMeshCount );
        }*/

        _backend->SetRenderTable( objectTable );

        // TODO: Within each group, sort by material.

        // TODO: For special items like fog and water, specialized calls will need to be made as these will require additional render passes.

        // TODO: vAfterward, do full-screen post fx

        // TODO: Finally render UI

        // If the frame preparation indicates we should wait, boot out early.
        if( !_backend->IsShutdown() ) {
            if( !_backend->PrepareFrame( deltaTime ) ) {
                return true;
            }

            return _backend->Frame( deltaTime );
        } else {
            return true;
        }
    }

    const bool RendererFrontEnd::UploadMeshData( const MeshUploadData& data, StaticMeshRenderReferenceData* referenceData ) {
        return _backend->UploadMeshData( data, referenceData );
    }

    void RendererFrontEnd::FreeMeshData( StaticMeshRenderReferenceData* referenceData ) {
        _backend->FreeMeshData( referenceData );
    }

    ITexture* RendererFrontEnd::GetTexture( const TString& name, const TString& path, const bool bypassCache ) {
        ITexture* texture;
        if( _textureCache->Exists( name ) ) {
            _textureCache->GetTextureReference( name, &texture );
        } else {
            texture = _backend->GetTexture( name, path );
            if( !bypassCache ) {
                _textureCache->Add( name, texture );
            }
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
            Logger::Warn( "Tried to release a texture not owned by the renderer: '%s'", name.CStr() );
        }
    }

    IShader* RendererFrontEnd::GetBuiltinMaterialShader( const MaterialType type ) {
        return _backend->GetBuiltinMaterialShader( type );
    }

    void swap( StaticMeshEntityComponent* a, StaticMeshEntityComponent* b ) {
        StaticMeshEntityComponent* temp = a;
        a = b;
        b = temp;
    }

    int partition( StaticMeshEntityComponent* arr[], I64 low, I64 high ) {
        StaticMeshEntityComponent* pivot = arr[high];
        I64 i = ( low - 1 );

        for( I64 j = low; j <= high - 1; ++j ) {
            // If current element is smaller than pivot, increment the low element.
            // TODO: Need to sort this by actual id versus just address.
            if( ( (StaticMeshRenderReferenceData*)arr[j]->GetReferenceData() )->Material <= ( (StaticMeshRenderReferenceData*)pivot )->Material ) {
                i++;
                swap( arr[i], arr[j] );
            }
        }
        swap( arr[i + 1], arr[high] );
        return ( i + 1 );
    }

    void quickSort( StaticMeshEntityComponent* arr[], I64 low, I64 high ) {
        if( low < high ) {
            I64 pivot = partition( arr, low, high );

            // Sort sub-arrays indepently
            quickSort( arr, low, pivot - 1 );
            quickSort( arr, pivot + 1, high );
        }
    }

    void RendererFrontEnd::sortByMaterialShader( StaticMeshEntityComponent** references, I32 count ) {
        quickSort( references, 0, count - 1 );
    }
}