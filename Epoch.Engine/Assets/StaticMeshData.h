#pragma once

#include "../Types.h"
#include "../Defines.h"
#include "../Math/Vector3.h"
#include "../String/TString.h"
#include "../FileSystem/IBinarySerializable.h"
#include "../Containers/List.h"
#include "../Renderer/Vertex3D.h"

namespace Epoch {

    enum class StaticMeshFileVersion : U8 {
        UNKNOWN = 0x00U,
        VERSION_1_0 = 0x01U
    };

    /**
     * Represents renderable sub-data within a static mesh. 
     */
    class EPOCH_API StaticMeshData : public IBinarySerializable {
    public:

        /**
         * The file version the mesh was loaded from.
         */
        U8 FormatVersion = 0;

        /**
         * The name of the object.
         */
        TString Name;

        /**
         * The object vertex data.
         */
        List<Vertex3D> Vertices;

        /**
         * The object index data.
         */
        List<U32> Indices;

        /**
         * Name of the material to be applied to this object.
         */
        TString MaterialName;

    public:

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