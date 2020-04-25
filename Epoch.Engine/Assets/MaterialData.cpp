
#include "MaterialData.h"
#include "../FileSystem/FileHandle.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger.h"

namespace Epoch {

    /*
    FORMAT:

    U8 FormatVersion;
    TString Name;
    TString DiffuseMapName;
    F32 Shininess;
    Vector3 SpecularColor;
    TString SpecularMapName;
    TString NormalMapName;
    TString EmissiveMapName;
    F32 Roughness;
    TString RoughnessMapName;
    F32 Metallic;
    TString MetallicMapName;
    */

    
    const bool MaterialData::SerializeBinary( const TString& filePath ) {
        FileHandle file( filePath, true );
        if( !file.TryOpen( FileMode::FILE_MODE_OUTPUT ) ) {
            Logger::Error( "Failed to open material file located at '%s'.", filePath.CStr() );
            return false;
        }

        // Format version.
        if( !file.Write<U8>( FormatVersion ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Name
        if( !file.Write<U32>( Name.Length() ) || !file.WriteString( Name ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Diffuse map name size
        if( !file.Write<U32>( DiffuseMapName.Length() ) || !file.WriteString( DiffuseMapName ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Shininess
        if( !file.Write<F32>( Shininess ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Specular color
        if( !file.Write<F32>( SpecularColor.X ) || !file.Write<F32>( SpecularColor.Y ) || !file.Write<F32>( SpecularColor.Z ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Specular map name
        if( !file.Write<U32>( SpecularMapName.Length() ) || !file.WriteString( SpecularMapName ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Normal map name
        if( !file.Write<U32>( NormalMapName.Length() ) || !file.WriteString( NormalMapName ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Emissive map name
        if( !file.Write<U32>( EmissiveMapName.Length() ) || !file.WriteString( EmissiveMapName ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Roughness
        if( !file.Write<F32>( Roughness ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Roughness map name
        if( !file.Write<U32>( RoughnessMapName.Length() ) || !file.WriteString( RoughnessMapName ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Metallic
        if( !file.Write<F32>( Metallic ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        // Metallic map name
        if( !file.Write<U32>( MetallicMapName.Length() ) || !file.WriteString( MetallicMapName ) ) {
            Logger::Error( "Error writing file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        }

        if( file.IsOpen() ) {
            file.Close();
        }

        Logger::Trace( "Material file written successfully." );
        return true;
    }

    
    const bool MaterialData::DeserializeBinary( const TString& filePath ) {
        FileHandle file( filePath, true );
        if( !file.TryOpen( FileMode::FILE_MODE_INPUT ) ) {
            Logger::Error( "Failed to open material file located at '%s'.", filePath.CStr() );
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
            if( FormatVersion != (U8)MaterialFileVersion::VERSION_1_0 ) {
                Logger::Error( "Unrecognized material file version." );
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

        // Diffuse map name size
        U32 diffuseMapLength;
        if( !file.Read<U32>( &diffuseMapLength ) || !file.ReadString( &DiffuseMapName, diffuseMapLength ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Diffuse map name (size): %s (%d)", DiffuseMapName.CStr(), diffuseMapLength );
        }

        // Shininess
        if( !file.Read<F32>( &Shininess ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Shininess: %f", Shininess );
        }

        // Specular color
        if( !file.Read<F32>( &SpecularColor.X ) || !file.Read<F32>( &SpecularColor.Y ) || !file.Read<F32>( &SpecularColor.Z ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Specular color: %s", SpecularColor.ToString() );
        }

        // Specular map name
        U32 specularMapLength;
        if( !file.Read<U32>( &specularMapLength ) || !file.ReadString( &SpecularMapName, specularMapLength ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Diffuse map name (size): %s (%d)", DiffuseMapName.CStr(), diffuseMapLength );
        }

        // Normal map name
        U32 normalMapLength;
        if( !file.Read<U32>( &normalMapLength ) || !file.ReadString( &NormalMapName, normalMapLength ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Normal map name (size): %s (%d)", NormalMapName.CStr(), normalMapLength );
        }

        // Emissive map name
        U32 emissiveMapLength;
        if( !file.Read<U32>( &emissiveMapLength ) || !file.ReadString( &EmissiveMapName, emissiveMapLength ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Emissive map name (size): %s (%d)", EmissiveMapName.CStr(), emissiveMapLength );
        }

        // Roughness
        if( !file.Read<F32>( &Roughness ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Shininess: %f", Roughness );
        }

        // Roughness map name
        U32 roughnessMapLength;
        if( !file.Read<U32>( &roughnessMapLength ) || !file.ReadString( &RoughnessMapName, roughnessMapLength ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Roughness map name (size): %s (%d)", RoughnessMapName.CStr(), roughnessMapLength );
        }

        // Metallic
        if( !file.Read<F32>( &Metallic ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Metallic: %f", Metallic );
        }

        // Metallic map name
        U32 metallicMapLength;
        if( !file.Read<U32>( &metallicMapLength ) || !file.ReadString( &MetallicMapName, metallicMapLength ) ) {
            Logger::Error( "Error reading file at position %d. Process aborted.", file.GetPosition() );
            file.Close();
            return false;
        } else {
            Logger::Trace( "Metallic map name (size): %s (%d)", MetallicMapName.CStr(), metallicMapLength );
        }

        if( file.IsOpen() ) {
            file.Close();
        }

        Logger::Trace( "Material file written successfully." );
        return true;
    }
}