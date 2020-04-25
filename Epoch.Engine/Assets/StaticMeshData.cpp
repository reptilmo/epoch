
#include "StaticMeshData.h"
#include "../FileSystem/FileHandle.h"
#include "../Logger.h"

namespace Epoch {


   /* StaticMeshData::StaticMeshData( const StaticMeshData& other ) {
        FormatVersion = other.FormatVersion;
        Name = other.Name;
        Vertices = List<Vertex3D>( other.Vertices.Size(), other.Vertices.Data() );
        Indices = List<U32>( other.Indices );
        MaterialName = other.MaterialName;
    }*/


    /*
    FORMAT:

    U8 format version
    U32 NameSize
    <NameSize> Name
    U32 MaterialNameSize
    <MaterialNameSize> MaterialName
    U32 VertCount
    <Vertex3D * VertCount> VertexData
    U32 IndexCount
    <IndexCount * U32> IndexData

    */


    // Do this first for path:
    /*
    TString extension = "esm";
    TString finalPath = TString::Format( "%s.%s", path.CStr(), extension.CStr() );
    */
    const bool StaticMeshData::SerializeBinary( const TString& filePath ) {
        FileHandle file( filePath, true );
        if( !file.TryOpen( FileMode::FILE_MODE_OUTPUT ) ) {
            Logger::Error( "Failed to open static mesh file located at '%s'." );
            return false;
        }

        // Format version.
        if( !file.Write<U8>( FormatVersion ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Name
        if( !file.Write<U32>( Name.Length() ) || !file.WriteString( Name ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Material name
        if( !file.Write<U32>( MaterialName.Length() ) || !file.WriteString( MaterialName ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Vertex count
        if( !file.Write<U32>( Vertices.Size() ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Write all vertex data
        U32 vertexCount = Vertices.Size();
        for( U32 i = 0; i < vertexCount; ++i ) {

            // Position
            if( !file.Write<F32>( Vertices[i].Position.X ) || !file.Write<F32>( Vertices[i].Position.Y ) || !file.Write<F32>( Vertices[i].Position.Z ) ) {
                Logger::Error( "Error reading vertex position at position: %d. Process aborted.", file.GetPosition() );
                file.Close();
                return false;
            }

            // Normal
            if( !file.Write<F32>( Vertices[i].Normal.X ) || !file.Write<F32>( Vertices[i].Normal.Y ) || !file.Write<F32>( Vertices[i].Normal.Z ) ) {
                Logger::Error( "Error reading vertex normal at position: %d. Process aborted.", file.GetPosition() );
                file.Close();
                return false;
            }

            // Texcoord
            if( !file.Write<F32>( Vertices[i].TexCoord.X ) || !file.Write<F32>( Vertices[i].TexCoord.Y ) ) {
                Logger::Error( "Error reading vertex texcoord at position: %d. Process aborted.", file.GetPosition() );
                file.Close();
                return false;
            }

            // Color
            if( !file.Write<F32>( Vertices[i].Color.X ) || !file.Write<F32>( Vertices[i].Color.Y ) || !file.Write<F32>( Vertices[i].Color.Z ) ) {
                Logger::Error( "Error reading vertex color at position: %d. Process aborted.", file.GetPosition() );
                file.Close();
                return false;
            }
        }

        // Index count
        U32 indexCount = Indices.Size();
        if( !file.Write<U32>( indexCount ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Read all index data
        for( U32 i = 0; i < indexCount; ++i ) {
            if( !file.Write<U32>( Indices[i] ) ) {
                Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
                file.Close();
                return false;
            }
        }

        if( file.IsOpen() ) {
            file.Close();
        }

        Logger::Trace( "File read successfully." );
        return true;
    }

    const bool StaticMeshData::DeserializeBinary( const TString& filePath ) {

        FileHandle file( filePath, true );
        if( !file.TryOpen( FileMode::FILE_MODE_INPUT ) ) {
            Logger::Error( "Failed to open static mesh file located at '%s'." );
            return false;
        }

        // Format version.
        if( !file.Read<U8>( &FormatVersion ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Format version: %d", FormatVersion );

            // NOTE: Add new versions here as they are available.
            if( FormatVersion != (U8)StaticMeshFileVersion::VERSION_1_0 ) {
                Logger::Error( "Unrecognized static mesh file version." );
            }
        }

        // Name
        U32 nameSize;
        if( !file.Read<U32>( &nameSize ) || !file.ReadString( &Name, nameSize ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Name (size): %s (%d)", Name.CStr(), nameSize );
        }

        // Material name
        U32 materialNameSize;
        if( !file.Read<U32>( &materialNameSize ) || !file.ReadString( &MaterialName, materialNameSize ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Name (size): %s (%d)", MaterialName.CStr(), materialNameSize );
        }

        // Vertex count
        U32 vertexCount;
        if( !file.Read<U32>( &vertexCount ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Vertex count: %d", vertexCount );
        }

        // Read all vertex data
        Vertices.Resize( vertexCount );
        for( U32 i = 0; i < vertexCount; ++i ) {

            // Position
            if( !file.Read<F32>( &Vertices[i].Position.X ) || !file.Read<F32>( &Vertices[i].Position.Y ) || !file.Read<F32>( &Vertices[i].Position.Z ) ) {
                Logger::Error( "Error reading vertex position at position: %d. Process aborted.", file.GetPosition() );
                file.Close();
                return false;
            }

            // Normal
            if( !file.Read<F32>( &Vertices[i].Normal.X ) || !file.Read<F32>( &Vertices[i].Normal.Y ) || !file.Read<F32>( &Vertices[i].Normal.Z ) ) {
                Logger::Error( "Error reading vertex normal at position: %d. Process aborted.", file.GetPosition() );
                file.Close();
                return false;
            }

            // Texcoord
            if( !file.Read<F32>( &Vertices[i].TexCoord.X ) || !file.Read<F32>( &Vertices[i].TexCoord.Y ) ) {
                Logger::Error( "Error reading vertex texcoord at position: %d. Process aborted.", file.GetPosition() );
                file.Close();
                return false;
            }

            // Color
            if( !file.Read<F32>( &Vertices[i].Color.X ) || !file.Read<F32>( &Vertices[i].Color.Y ) || !file.Read<F32>( &Vertices[i].Color.Z ) ) {
                Logger::Error( "Error reading vertex color at position: %d. Process aborted.", file.GetPosition() );
                file.Close();
                return false;
            }
        }

        // Index count
        U32 indexCount;
        if( !file.Read<U32>( &indexCount ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Index count: %d", indexCount );
        }

        // Read all index data
        Indices.Resize( indexCount );
        for( U32 i = 0; i < indexCount; ++i ) {
            if( !file.Read<U32>( &Indices[i] ) ) {
                Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
                file.Close();
                return false;
            }
        }

        if( file.IsOpen() ) {
            file.Close();
        }

        Logger::Trace( "File read successfully." );
        return true;
    }
}