
#include "../Renderer/Frontend/RendererFrontend.h"
#include "../Renderer/Material.h"
#include "../Logger.h"
#include "StaticMesh.h"

namespace Epoch {

    StaticMesh::StaticMesh() {
        _name = "";
        _path = "";
    }

    StaticMesh::StaticMesh( const TString& name, const TString& filePath ) {
        _name = name;
        _path = filePath;
    }

    StaticMesh::~StaticMesh() {
        _name.Clear();
        _path.Clear();
    }

    const bool StaticMesh::Load() {
        if( _isLoaded ) {
            return true;
        }

        Logger::Trace( "Loading static mesh '%s' from file '%s'", _name.CStr(), _path.CStr() );
        const bool result = _data.DeserializeBinary( _path );

        Logger::Log( "StaticMesh::loadMeshDataFromFile() - Mesh verts require %.2f MiB", ( (F32)_data.Vertices.Size() / 1024.0f / 1024.0f ) );
        Logger::Log( "StaticMesh::loadMeshDataFromFile() - Mesh indices require %.2f MiB", ( (F32)_data.Indices.Size() / 1024.0f / 1024.0f ) );

        uploadToGPU();

        _isLoaded = true;

        return result;
    }

    void StaticMesh::Unload() {
        _data.Vertices.Clear();
        _data.Indices.Clear();
        _data.MaterialName.Clear();
        _data.Name.Clear();

        if( _isLoaded ) {
            RendererFrontEnd::FreeMeshData( &_referenceData );
        }

        _isLoaded = false;
    }

    const bool StaticMesh::uploadToGPU() {

        Logger::Trace( "Uploading mesh data to GPU..." );

        if( _data.Vertices.Size() == 0 || _data.Indices.Size() == 0 ) {
            return false;
        }

        MeshUploadData uploadData;
        uploadData.Vertices = _data.Vertices;
        uploadData.Indices = _data.Indices;

        // Upload and retrieve the reference data.
        if( !RendererFrontEnd::UploadMeshData( uploadData, &_referenceData ) ) {
            Logger::Error( "Error uploading mesh. See logs for details." );
            return false;
        }

        // Load/assign materials.
        if( MaterialManager::Exists( _data.MaterialName ) ) {
            _referenceData.Material = MaterialManager::Get( _data.MaterialName );
        } else {
            _referenceData.Material = MaterialManager::Get( "__DEFAULT__" );
        }

        Logger::Trace( "Mesh loading complete: %s", _path.CStr() );
        return true;
    }
}