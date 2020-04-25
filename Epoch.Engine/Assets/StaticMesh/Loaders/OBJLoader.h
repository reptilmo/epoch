#pragma once

#include "../../../Defines.h"

#include "../../StaticMeshData.h"
#include "../../MaterialData.h"

namespace Epoch {

    class TString;


    /**
     * Used to load OBJ files.
     */
    class EPOCH_API OBJLoader {
    public:

        /**
         * Loads OBJ file contents into a collection of meshes. This is done synchronously and returns immediately.
         *
         * @param path The path to the file.
         *
         * @returns A collection of mesh data.
         */
        static const bool LoadObjFile( const TString& path, U32* meshCount, StaticMeshData** meshes, U32* materialCount, MaterialData** materials );
    };
}