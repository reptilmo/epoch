
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../External/tiny_obj_loader/tiny_obj_loader.h"

#include "../../../Renderer/Vertex3D.h"
#include "OBJLoader.h"

namespace Epoch {

    std::vector<StaticMesh> OBJLoader::LoadObjFile( const std::string& path ) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if( !tinyobj::LoadObj( &attrib, &shapes, &materials, &warn, &err, path.c_str(), "assets/models/" ) ) {
            throw std::runtime_error( warn + err );
        }

        std::vector<StaticMesh> meshes;

        std::unordered_map<Vertex3D, U32> uniqueVertices = {};

        U64 vertexArrayIndex = 0;

        for( const auto& shape : shapes ) {
            StaticMesh mesh;
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
            meshes.push_back( mesh );
        }

        return meshes;
    }
}
