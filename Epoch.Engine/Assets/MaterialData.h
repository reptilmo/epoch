#pragma once

#include "../Types.h"
#include "../Defines.h"
#include "../Math/Vector3.h"
#include "../String/TString.h"
#include "../FileSystem/IBinarySerializable.h"

namespace Epoch {

    enum class MaterialFileVersion : U8 {
        UNKNOWN = 0x00U,
        VERSION_1_0 = 0x01U
    };

    /**
     * Material info to be used with a static mesh data object.
     */
    class EPOCH_API MaterialData : public IBinarySerializable {
    public:
        MaterialData();
        MaterialData( const MaterialData& other ) {
            FormatVersion = other.FormatVersion;
            Name = other.Name;
            DiffuseMapName = other.DiffuseMapName;
            Shininess = other.Shininess;
            SpecularColor = other.SpecularColor;
            SpecularMapName = other.SpecularMapName;
            NormalMapName = other.NormalMapName;
            EmissiveMapName = other.EmissiveMapName;
            Roughness = other.Roughness;
            RoughnessMapName = other.RoughnessMapName;
            Metallic = other.Metallic;
            MetallicMapName = other.MetallicMapName;
        }
    public:

        /**
         * Material file version.
         */
        U8 FormatVersion;

        /**
         * The name of the sub-object.
         */
        TString Name;

        /**
         * The name of the diffuse map.
         */
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

        /**
         * Binary-serializes the contents of this structure to a file at the given path.
         *
         * @param filePath The path of the file to serialize to.
         *
         * @returns True if successful; otherwise false.
         */
        const bool SerializeBinary( const TString& filePath );

        /**
         * Binary-deserializes the contents of this structure from the file at the given path.
         *
         * @param filePath The path of the file to deserialize from.
         *
         * @returns True if successful; otherwise false.
         */
        const bool DeserializeBinary( const TString& filePath );
    };
}