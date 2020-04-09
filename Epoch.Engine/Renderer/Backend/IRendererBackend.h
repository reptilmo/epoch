#pragma once

#include "../../Types.h"
#include "../MeshData.h"

namespace Epoch {

    enum class MaterialType;
    class TString;
    class ITexture;
    class IShader;

    /**
     * Represents the backend of the renderer, which is an abstraction of the
     * underlying graphics API (such as Vulkan, DirectX or OpenGL)
     */
    class IRendererBackend {
    public:

        virtual ~IRendererBackend() {}

        /**
         * Initializes this renderer.
         * 
         * @returns True if successful; otherwise false.
         */
        virtual const bool Initialize() = 0;

        /**
         * Destroys this renderer, releasing all of its resources.
         */
        virtual void Destroy() = 0;

        /**
         * Performs operations required for the next frame render.
         *
         * @param deltaTime The amount of time in seconds since the last frame.
         *
         * @returns True if Frame() should be called; otherwise false.
         */
        virtual const bool PrepareFrame( const F32 deltaTime ) = 0;

        /**
         * Processes a single frame.
         * 
         * @param deltaTime The amount of time in seconds since the last frame.
         * 
         * @returns True on success, false on failure. Returning false crashes the application.
         */
        virtual const bool Frame( const F32 deltaTime ) = 0;


        virtual const bool UploadMeshData( const MeshUploadData& data, MeshRendererReferenceData* referenceData ) = 0;

        virtual void FreeMeshData( const U64 index ) = 0;

        virtual void AddToFrameRenderList( const MeshRendererReferenceData* referenceData ) = 0;

        /**
         * Returns a new texture from this renderer. This is always a creation request, as the front end
         * should have already determined if a texture with the given name exists at this point.
         */
        virtual ITexture* GetTexture( const TString& name, const TString& path ) = 0;

        virtual IShader* GetBuiltinMaterialShader( const MaterialType type ) = 0;
    };
}