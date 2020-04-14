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
         * Indicates if this backend is shut down.
         */
        virtual const bool IsShutdown() const = 0;

        /**
         * Initializes this renderer.
         * 
         * @returns True if successful; otherwise false.
         */
        virtual const bool Initialize() = 0;

        /**
         * Flags this renderer as shut down and begins the shutdown process. Must be invoked before Destroy or delete.
         */
        virtual void Shutdown() = 0;

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

        /**
         * Uploads the provided mesh data to this backend and sets the render API reference data in the provided referenceData object.
         * 
         * @param data A reference to the data to be uploaded.
         * @param referenceData A pointer to the reference data object which will hold the API reference info.
         * 
         * @returns True if successful; otherwise false.
         */
        virtual const bool UploadMeshData( const MeshUploadData& data, MeshRendererReferenceData* referenceData ) = 0;

        /**
         * Frees mesh data using the provided reference data. 
         *
         * @param referenceData A pointer to the reference data object whose data should be released.
         */
        virtual void FreeMeshData( MeshRendererReferenceData* referenceData ) = 0;

        /**
         * Adds the object referenced by the provided reference data to a list of objects to be rendered in the next frame.
         * 
         * @param referenceData A pointer to the reference data object to be rendered.
         */
        virtual void AddToFrameRenderList( const MeshRendererReferenceData* referenceData ) = 0;

        /**
         * Returns a new texture from this renderer. This is always a creation request, as the front end
         * should have already determined if a texture with the given name exists at this point.
         *
         * @param name The name of the texture.
         * @param path The file path of the texture.
         *
         * @returns A pointer to the newly-created texture.
         */
        virtual ITexture* GetTexture( const TString& name, const TString& path ) = 0;

        /**
         * Obtains a pointer to a builtin shader of the given type. Only one builtin shader of each type exists.
         * 
         * @param type The type whose shader to retrieve.
         *
         * @returns A pointer to the builtin shader. Can return nullptr if not found.
         */
        virtual IShader* GetBuiltinMaterialShader( const MaterialType type ) = 0;
    };
}