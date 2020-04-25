#pragma once

#include "../Types.h"
#include "../String/TString.h"
#include "../Math/Transform.h"
#include "../Assets/StaticMeshData.h"

#include "../Containers/List.h"
#include "../Renderer/Vertex3D.h"
#include "../Renderer/RenderData.h"

namespace Epoch {

    class BaseMaterial;

    /**
     * Renderer reference data used for rendering a static mesh.
     */
    struct StaticMeshRenderReferenceData : public RenderReferenceData {
        U64 VertexHeapIndex;
        U64 IndexHeapIndex;

        BaseMaterial* Material = nullptr;

        StaticMeshRenderReferenceData() : RenderReferenceData( RenderableComponentType::STATIC_MESH ) {

        }

        StaticMeshRenderReferenceData( const StaticMeshRenderReferenceData& other ) : RenderReferenceData( other ) {
            VertexHeapIndex = other.VertexHeapIndex;
            IndexHeapIndex = other.IndexHeapIndex;
            Material = other.Material;
        }

        ~StaticMeshRenderReferenceData() {
            VertexHeapIndex = -1;
            IndexHeapIndex = -1;
            Material = nullptr;
        }
    };

    /**
     * A structure used to upload mesh data to the GPU.
     */
    struct MeshUploadData {
        List<Vertex3D> Vertices;
        List<U32> Indices;
    };

    /**
     * Represents a static mesh in the world.
     */
    class StaticMesh {
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
         */
        StaticMesh( const TString& name, const TString& filePath );

        /**
         * Default destructor.
         */
        virtual ~StaticMesh();

        /**
         * Loads resources required by this static mesh. Also uploads this mesh to the GPU.
         */
        virtual const bool Load();

        /**
         * Unloads this mesh's data from the GPU. Does not clear index/vertex data.
         */
        virtual void Unload();

        /**
         * Retrieves the renderer reference data for this mesh.
         *
         * @returns A pointer to the mesh bject's renderer reference data.
         */
        StaticMeshRenderReferenceData* GetReferenceData() noexcept { return &_referenceData; }

        /**
         * Indicates if this mesh has been loaded.
         */
        const bool IsLoaded() const noexcept { return _isLoaded; }

    private:
        const bool uploadToGPU();

    private:
        TString _name;
        TString _path;
        bool _isLoaded = false;
        StaticMeshData _data;
        StaticMeshRenderReferenceData _referenceData;
    };
}