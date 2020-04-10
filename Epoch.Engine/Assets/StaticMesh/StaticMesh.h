#pragma once

#include <vector>

#include "../../Types.h"
#include "../../String/TString.h"
#include "../../Events/IEventHandler.h"

#include "../../Renderer/Vertex3D.h"
#include "../../Renderer/MeshData.h"

namespace Epoch {

    class BaseMaterial;

    struct MaterialInfo {
        TString Name;
        TString DiffuseMapName;
    };

    struct StaticMeshData {
        TString Name;
        std::vector<Epoch::Vertex3D> Vertices;
        std::vector<U32> Indices;

        MeshRendererReferenceData RendererReferenceData;

        MaterialInfo MaterialInfo;
    };

    class StaticMesh : public IEventHandler {
    public:
        static StaticMesh* FromFile( const TString& name, const TString& filePath, const bool autoUploadToGPU = true );

    public:
        StaticMesh();
        StaticMesh( const TString& name, const TString& filePath, const bool autoUploadToGPU = true );
        virtual ~StaticMesh();
        virtual void OnEvent( const Event* event ) override;

        const MeshRendererReferenceData* GetMeshReferenceData( const U64 index ) const noexcept { return &_meshes[index].RendererReferenceData; }

        const U64 GetMeshDataCount() const noexcept { return _meshes.size(); }

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