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
        std::string Name;
        std::vector<Epoch::Vertex3D> Vertices;
        std::vector<U32> Indices;

        MeshRendererReferenceData RendererReferenceData;

        MaterialInfo MaterialInfo;
    };

    class StaticMesh : public IEventHandler {
    public:
        static StaticMesh* FromFile( const std::string name, const std::string filePath, const bool autoUploadToGPU = true );

    public:

        virtual void OnEvent( const Event* event );

        const MeshRendererReferenceData* GetMeshReferenceData( const U64 index ) const { return &_meshes[index].RendererReferenceData; }

        const U64 GetMeshDataCount() const { return _meshes.size(); }

        const bool IsUploaded() const { return _isUploaded; }

    private:
        const bool loadMeshDataFromFile( const std::string& name, const std::string& filePath );
        const bool uploadToGPU();
        void onDataLoaded( std::vector<StaticMeshData> data );

    private:
        std::string _name;
        std::string _path;
        bool _autoUpload = false;
        bool _isUploaded = false;
        std::vector<StaticMeshData> _meshes;
    };
}