
#include <unordered_map>
#include <thread>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../External/tiny_obj_loader/tiny_obj_loader.h"

#include "../../../Renderer/Vertex3D.h"
#include "../../../Events/Event.h"

#include "../../AssetLoadedEvent.h"

#include "../StaticMeshAssetData.h"
#include "../StaticMesh.h"

#include "OBJLoader.h"

namespace Epoch {

    void OBJLoader::LoadObjFile( const std::string& name, const std::string& path ) {

        std::thread processThread( OBJLoader::loadOnThread, name, path );
        processThread.detach();
    }

    void OBJLoader::loadOnThread( const std::string& name, const std::string& path ) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if( !tinyobj::LoadObj( &attrib, &shapes, &materials, &warn, &err, path.c_str(), "assets/models/" ) ) {
            throw std::runtime_error( warn + err );
        }

        std::vector<StaticMeshData> meshes;

        std::unordered_map<Vertex3D, U32> uniqueVertices = {};

        U64 vertexArrayIndex = 0;

        for( const auto& shape : shapes ) {
            StaticMeshData mesh;
            mesh.Name = shape.name;

            for( const auto& index : shape.mesh.indices ) {
                Vertex3D vertex = {};

                // NOTE: attrib.vertices is a float[] 
                vertex.Position.X = attrib.vertices[3 * index.vertex_index + 0];
                vertex.Position.Y = attrib.vertices[3 * index.vertex_index + 1];
                vertex.Position.Z = attrib.vertices[3 * index.vertex_index + 2];

                vertex.TexCoord.X = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.TexCoord.Y = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];

                if( index.normal_index != -1 ) {
                    vertex.Normal.X = attrib.normals[3 * index.normal_index + 0];
                    vertex.Normal.Y = attrib.normals[3 * index.normal_index + 1];
                    vertex.Normal.Z = attrib.normals[3 * index.normal_index + 2];
                }

                // TODO: Color
                // vertex.r = vertex.g = vertex.b = 1.0f;
                vertex.Color.X = attrib.colors[3 * index.vertex_index + 0];
                vertex.Color.Y = attrib.colors[3 * index.vertex_index + 1];
                vertex.Color.Z = attrib.colors[3 * index.vertex_index + 2];

                // Remove duplicate vertices.
                if( uniqueVertices.count( vertex ) == 0 ) {
                    uniqueVertices[vertex] = (U32)( mesh.Vertices.size() );
                    mesh.Vertices.push_back( vertex );
                }
                //_indices.push_back( _indices.size() );
                mesh.Indices.push_back( uniqueVertices[vertex] );

                vertexArrayIndex++;
            }

            // Only push back meshes with actual data in them.
            if( mesh.Vertices.size() != 0 && mesh.Indices.size() != 0 ) {
                meshes.push_back( mesh );
            }
        }

        // Execute the callback.
        StaticMeshAssetData* assetData = new StaticMeshAssetData( path, name, meshes );

        // Post the event, but *not* immediate. By waiting until the next frame, this thread no longer has
        // a hold on the data.
        AssetLoadedEvent* loadedEvent = new AssetLoadedEvent( assetData, nullptr );
        loadedEvent->Post( false );
    }
}
