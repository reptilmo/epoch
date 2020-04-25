#pragma once

#include "../Defines.h"

namespace Epoch {

    class TString;

    /**
     * Provides method signatures for serialization and binary deserialization.
     */
    class EPOCH_API IBinarySerializable {
    public:
        virtual ~IBinarySerializable() {}

        /**
         * Binary-serializes the contents of this structure to a file at the given path.
         * 
         * @param filePath The path of the file to serialize to.
         * 
         * @returns True if successful; otherwise false.
         */
        virtual const bool SerializeBinary( const TString& filePath ) = 0;

        /**
         * Binary-deserializes the contents of this structure from the file at the given path.
         *
         * @param filePath The path of the file to deserialize from.
         *
         * @returns True if successful; otherwise false.
         */
        virtual const bool DeserializeBinary( const TString& filePath ) = 0;
    };
}