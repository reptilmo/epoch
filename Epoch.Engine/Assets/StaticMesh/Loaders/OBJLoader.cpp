
#include <unordered_map>
#include <vector>
//#include <thread>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../External/tiny_obj_loader/tiny_obj_loader.h"

#include "../../../String/TString.h"
#include "../../../Renderer/Vertex3D.h"
#include "../../../Events/Event.h"
#include "../../../Renderer/Material.h"
#include "../../../Logger.h"

#include "../../Asset.h"

#include "OBJLoader.h"

namespace Epoch {

    const bool OBJLoader::LoadObjFile( const TString& path, U32* meshCount, StaticMeshData** meshes, U32* materialCount, MaterialData** materials ) {

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> rawMaterials;
        std::string warn, err;

        if( !tinyobj::LoadObj( &attrib, &shapes, &rawMaterials, &warn, &err, path.CStr(), "assets/models/" ) ) {
            Logger::Error( ( warn + err ).c_str() );
            return false;
        }

        ( *meshes ) = static_cast<StaticMeshData*>( TMemory::Allocate( sizeof( StaticMeshData ) * shapes.size() ) );
        TMemory::MemZero( ( *meshes ), sizeof( StaticMeshData ) * shapes.size() );
        ( *materials ) = static_cast<MaterialData*>( TMemory::Allocate( sizeof( MaterialData ) * rawMaterials.size() ) );
        TMemory::MemZero( ( *materials ), sizeof( MaterialData ) * rawMaterials.size() );

        // Fill out materials first
        U32 index = 0;
        for( auto material : rawMaterials ) {
            //MaterialData matInfo = *materials[index];
            ( *materials[index] ).FormatVersion = (U8)MaterialFileVersion::VERSION_1_0;
            ( *materials[index] ).Name = material.name.c_str();
            ( *materials[index] ).DiffuseMapName = material.diffuse_texname.c_str();

            ( *materials[index] ).Shininess = material.shininess;

            // Specular
            ( *materials[index] ).SpecularColor.Set( material.specular[0], material.specular[1], material.specular[2] );
            if( !material.specular_texname.empty() ) {
                ( *materials[index] ).SpecularMapName = material.specular_texname.c_str();
            }

            // Normal
            if( !material.normal_texname.empty() ) {
                ( *materials[index] ).NormalMapName = material.normal_texname.c_str();
            }

            // Emissive
            if( !material.emissive_texname.empty() ) {
                ( *materials[index] ).EmissiveMapName = material.emissive_texname.c_str();
            }

            // Roughness
            ( *materials[index] ).Roughness = material.roughness;
            if( !material.roughness_texname.empty() ) {
                ( *materials[index] ).RoughnessMapName = material.roughness_texname.c_str();
            }

            // Metallic
            ( *materials[index] ).Metallic = material.metallic;
            if( !material.metallic_texname.empty() ) {
                ( *materials[index] ).MetallicMapName = material.metallic_texname.c_str();
            }

            // TODO: May want to add clearcoat, etc.
            ( *materialCount )++;
            ++index;
        }

        std::unordered_map<Vertex3D, U32> uniqueVertices = {};

        U64 vertexArrayIndex = 0;
        index = 0;
        for( const auto& shape : shapes ) {
            ( *meshes[index] ).FormatVersion = (U8)StaticMeshFileVersion::VERSION_1_0;
            //StaticMeshData mesh = *meshes[index];
            ( *meshes[index] ).Name = shape.name.c_str();

            // Parse material info. This engine will not support per-face materials. 
            // Therefore, just use the material id from then first face. 
            if( !shape.mesh.material_ids.empty() ) {
                ( *meshes[index] ).MaterialName = ( *materials[shape.mesh.material_ids[0]] ).Name;
            }

            // Process the geometry
            for( const auto& meshIndex : shape.mesh.indices ) {
                Vertex3D vertex = {};

                // NOTE: attrib.vertices is a float[] 
                vertex.Position.X = attrib.vertices[3 * (I64)meshIndex.vertex_index + 0];
                vertex.Position.Y = attrib.vertices[3 * (I64)meshIndex.vertex_index + 1];
                vertex.Position.Z = attrib.vertices[3 * (I64)meshIndex.vertex_index + 2];

                vertex.TexCoord.X = attrib.texcoords[2 * (I64)meshIndex.texcoord_index + 0];
                vertex.TexCoord.Y = 1.0f - attrib.texcoords[2 * (I64)meshIndex.texcoord_index + 1];

                if( meshIndex.normal_index != -1 ) {
                    vertex.Normal.X = attrib.normals[3 * (I64)meshIndex.normal_index + 0];
                    vertex.Normal.Y = attrib.normals[3 * (I64)meshIndex.normal_index + 1];
                    vertex.Normal.Z = attrib.normals[3 * (I64)meshIndex.normal_index + 2];
                }

                // TODO: Color
                // vertex.r = vertex.g = vertex.b = 1.0f;
                vertex.Color.X = attrib.colors[3 * (I64)meshIndex.vertex_index + 0];
                vertex.Color.Y = attrib.colors[3 * (I64)meshIndex.vertex_index + 1];
                vertex.Color.Z = attrib.colors[3 * (I64)meshIndex.vertex_index + 2];

                // Remove duplicate vertices.
                if( uniqueVertices.count( vertex ) == 0 ) {
                    uniqueVertices[vertex] = (U32)( *meshes[index] ).Vertices.Size();
                    ( *meshes[index] ).Vertices.Add( vertex );
                }
                //_indices.push_back( _indices.size() );
                ( *meshes[index] ).Indices.Add( uniqueVertices[vertex] );

                vertexArrayIndex++;
            }

            // Only push back meshes with actual data in them.
            //if( mesh.Vertices.Size() != 0 && mesh.Indices.Size() != 0 ) {
            ( *meshCount )++;
            //}
        }

        return true;
    }
}
