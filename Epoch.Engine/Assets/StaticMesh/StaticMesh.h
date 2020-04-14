#pragma once

#include <vector>

#include "../../Types.h"
#include "../../String/TString.h"
#include "../../Events/IEventHandler.h"

#include "../../Renderer/Vertex3D.h"
#include "../../Renderer/MeshData.h"

namespace Epoch {

    class BaseMaterial;

    /**
     * Material info to be used with a static mesh data object.
     */
    struct MaterialInfo {

        /**
         * The name of the sub-object.
         */
        TString Name;

        /**
         * The name of the diffuse map.
         */
        TString DiffuseMapName;
    };

    /**
     * Represents renderable sub-data within a static mesh.
     */
    struct StaticMeshData {

        /**
         * The name of the sub-object.
         */
        TString Name;

        /**
         * The sub-object vertex data.
         */
        std::vector<Epoch::Vertex3D> Vertices;

        /**
         * The sub-object index data.
         */
        std::vector<U32> Indices;

        /**
         * The renderer reference data, only filled out once uploaded to the GPU.
         */
        MeshRendererReferenceData RendererReferenceData;

        /**
         * Material reference info for this sub-object.
         */
        MaterialInfo MaterialInfo;
    };

    /**
     * Represents a static mesh in the world.
     */
    class StaticMesh : public IEventHandler {
    public:

        /**
         * Loads a static mesh from the given file path.
         * 
         * @param name The name of the mesh.
         * @param filePath The path to the file containing the mesh data.
         * @param autoUploadToGPU Indicates if this mesh should be immediately uploaded to the GPU when loaded. Default: true.
         * 
         * @returns A pointer to the newly-created static mesh.
         */
        static StaticMesh* FromFile( const TString& name, const TString& filePath, const bool autoUploadToGPU = true );

    public:

        /**
         * Default constructor. Should not be used in most cases.
         */
        StaticMesh();

        /**
         * Creates a new StaticMesh.
         *
         * @param name The name of the mesh.
         * @param filePath The path to the file containing the mesh data.
         * @param autoUploadToGPU Indicates if this mesh should be immediately uploaded to the GPU when loaded. Default: true.
         */
        StaticMesh( const TString& name, const TString& filePath, const bool autoUploadToGPU = true );
        
        /**
         * Default destructor.
         */
        virtual ~StaticMesh();

        /**
         * Processes the given event.
         *
         * @param event The event to be processed.
         */
        virtual void OnEvent( const Event* event ) override;

        /**
         * Unloads this mesh's data from the GPU. Does not clear index/vertex data.
         */
        virtual void Unload();

        /**
         * Retrieves the renderer reference data for the sub-object at the provided index.
         *
         * @param index The index of the reference data to retrieve.
         *
         * @returns A pointer to the mesh sub-object's renderer reference data.
         */
        const MeshRendererReferenceData* GetMeshReferenceData( const U64 index ) const noexcept { return &_meshes[index].RendererReferenceData; }

        /**
         * Returns the count of sub-objects present in this static mesh.
         */
        const U64 GetMeshDataCount() const noexcept { return _meshes.size(); }

        /**
         * Indicates if this mesh's data has been uploaded to the GPU.
         */
        const bool IsUploaded() const noexcept { return _isUploaded; }

    private:
        const bool loadMeshDataFromFile( const TString& name, const TString& filePath );
        const bool uploadToGPU();
        void onDataLoaded( std::vector<StaticMeshData> data );

    private:
        TString _name;
        TString _path;
        bool _autoUpload = false;
        bool _isUploaded = false;
        std::vector<StaticMeshData> _meshes;
    };
}