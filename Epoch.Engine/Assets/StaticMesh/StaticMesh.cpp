
#include "../AssetLoadedEvent.h"
#include "../../Logger.h"

#include "../../Renderer/Frontend/RendererFrontend.h"
#include "StaticMeshAssetData.h"
#include "Loaders/OBJLoader.h"
#include "StaticMesh.h"

namespace Epoch {

    StaticMesh* StaticMesh::FromFile( const std::string name, const std::string filePath, const bool autoUploadToGPU ) {

        StaticMesh* mesh = new StaticMesh();
        mesh->_name = name;
        mesh->_path = filePath;
        mesh->_autoUpload = autoUploadToGPU;

        // This is asynchronous and does not return a result here.
        mesh->loadMeshDataFromFile( name, filePath );

        return mesh;
    }

    void StaticMesh::OnEvent( const Event* event ) {
        if( event->Type == EventType::ASSET_LOADED ) {
            const AssetLoadedEvent* typedEvent = static_cast<const AssetLoadedEvent*>( event );
            if( typedEvent->Data->GetName() == _name ) {
                // The correct asset has loaded, stop listening for asset loaded events.
                Event::StopListening( EventType::ASSET_LOADED, this );

                // Load the data.
                const StaticMeshAssetData* typedData = static_cast<const StaticMeshAssetData*>( typedEvent->Data );
                onDataLoaded( typedData->GetData() );
            }
        }
    }

    const bool StaticMesh::loadMeshDataFromFile( const std::string& name, const std::string& filePath ) {
        //_meshes = OBJLoader::LoadObjFile( "assets/models/sibenik.obj" );

        // Listen for an asset loaded event.
        Event::Listen( EventType::ASSET_LOADED, this );

        // TODO: Handle failed mesh loading and other formats.
        OBJLoader::LoadObjFile( name, filePath );

        return true;
    }

    const bool StaticMesh::uploadToGPU() {

        Logger::Trace( "Uploading mesh data to GPU..." );

        bool success = true;

        for( StaticMeshData& mesh : _meshes ) {
            if( mesh.Vertices.size() == 0 || mesh.Indices.size() == 0 ) {
                continue;
            }

            MeshUploadData uploadData;
            uploadData.Vertices = mesh.Vertices;
            uploadData.Indices = mesh.Indices;

            // Upload and retrieve the reference data.
            if( !RendererFrontEnd::UploadMeshData( uploadData, &mesh.RendererReferenceData ) ) {
                success = false;
                Logger::Error( "Error uploading mesh. See logs for details." );
                return false;
            }
        }

        _isUploaded = true;
        Logger::Trace( "Done." );
    }

    void StaticMesh::onDataLoaded( std::vector<StaticMeshData> data ) {
        _meshes = data;
        if( _autoUpload ) {

            U64 totalVertSize = 0;
            U64 totalIndexSize = 0;
            for( auto mesh : _meshes ) {
                totalVertSize += ( sizeof( Vertex3D ) * mesh.Vertices.size() );
                totalIndexSize += ( sizeof( U32 ) * mesh.Indices.size() );
            }
            Logger::Log( "StaticMesh::loadMeshDataFromFile() - Mesh verts require %.2f MiB", ( (F32)totalVertSize / 1024.0f / 1024.0f ) );
            Logger::Log( "StaticMesh::loadMeshDataFromFile() - Mesh indices require %.2f MiB", ( (F32)totalIndexSize / 1024.0f / 1024.0f ) );

            uploadToGPU();
        }
    }
}