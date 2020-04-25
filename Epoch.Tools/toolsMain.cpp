
#include <String/TString.h>
#include <Containers/List.h>
#include <Assets/StaticMesh/Loaders/OBJLoader.h>
#include <Renderer/Material.h>
#include <FileSystem/FileSystem.h>
#include <Assets/MaterialData.h>
#include <Logger.h>

using namespace Epoch;

int main( int argc, const char* argv[] ) {

    // Make arguments easily digestible.
    List<TString> arguments( true );
    for( int i = 0; i < argc; ++i ) {
        arguments.Add( argv[i] );
    }

    // TODO: assuming OBJ file conversion for now.
    if( true ) {
        TString name = "rubbish";
        TString filePath = "assets/models/teapot.obj";
        U32 meshCount = 0;
        U32 materialCount = 0;
        StaticMeshData* meshes;
        MaterialData* materials;
        if( !OBJLoader::LoadObjFile( filePath, &meshCount, &meshes, &materialCount, &materials ) ) {
            return 1;
        }

        // Write materials.
        for( U32 i = 0; i < materialCount; ++i ) {
            //MaterialData material = materials[i];

            // TODO: custom output path
            TString matPath = "assets/materials/";
            matPath.Append( materials[i].Name );
            matPath.Append( EPOCH_FILE_EXT_MATERIAL );

            Logger::Log( "Writing material file: %s", matPath.CStr() );

            // Write the file.
            MaterialData material = materials[i];
            material.SerializeBinary( matPath );
        }

        // Write static meshes.
        for( U32 i = 0; i < meshCount; ++i ) {
            // TODO: custom output path
            TString meshPath = "assets/models/";
            meshPath.Append( meshes[i].Name );
            meshPath.Append( EPOCH_FILE_EXT_STATIC_MESH );

            Logger::Log( "Writing static mesh file: %s", meshPath.CStr() );

            StaticMeshData staticMesh = meshes[i];
            staticMesh.SerializeBinary( meshPath );
        }

        TMemory::Free( meshes );
        TMemory::Free( materials );

    }
}