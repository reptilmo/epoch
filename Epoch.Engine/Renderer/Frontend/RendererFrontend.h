#pragma once

#include "../../Types.h"

namespace Epoch {

    enum class MaterialType;
    struct MeshUploadData;
    struct MeshRendererReferenceData;

    class Engine;
    class TString;
    class IRendererBackend;
    class TextureCache;
    class ITexture;
    class IShader;

    /*
     The renderer "front-end", which represents the interaction point with the rest of the engine.
     This is responsible for tasks such as scene object sorting, object culling (frustum, occlusion, etc)
     as well as building interaction tables to determine how lights interact with surfaces. This information
     is passed to this object's counterpart, the "back-end", which makes the required calls to the underlying
     grapics API.
    */
    class RendererFrontEnd final {
    public:

        /**
         * Initializes this renderer.
         *         
         * @param engine A pointer to the engine which owns this renderer.
         *
         * @returns True if successful; otherwise false.
         */
        static const bool Initialize( Engine* engine );

        /**
         * Shuts the rendering system down and releases all held resources.
         */
        static void Shutdown();

        /**
         * Processes a single frame.
         *
         * @param deltaTime The amount of time in seconds since the last frame.
         *
         * @returns True on success, false on failure. Returning false crashes the application.
         */
        static const bool Frame( const F32 deltaTime );

        static const bool UploadMeshData( const MeshUploadData& data, MeshRendererReferenceData* referenceData );

        static void FreeMeshData( const U64 index );

        /**
         * Obtains a texture reference to the provided name. Path, for now, is required.
         * TODO: Use discovery/manifest so the path isn't required here.
         */
        static ITexture* GetTexture( const TString& name, const TString& path, const bool bypassCache = false );

        static ITexture* GetDefaultWhiteTexture();

        /**
         * Releases a reference to the texture with the given name. If no references remain, the texture is unloaded.
         */
        static void ReleaseTexture( const TString& name );

        static IShader* GetBuiltinMaterialShader( const MaterialType type );

    private:
        // Remove the ability to instantiate this class.
        RendererFrontEnd() {}
        ~RendererFrontEnd() {}

    private:

        // A pointer to the engine which owns this renderer.
        static Engine* _engine;

        // A pointer to the back end.
        static IRendererBackend* _backend;

        static TextureCache* _textureCache;
    };
}