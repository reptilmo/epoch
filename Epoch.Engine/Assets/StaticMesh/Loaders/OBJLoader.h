#pragma once

#include <vector>
#include <string>

#include "../StaticMesh.h"

namespace Epoch {

    /**
     * Used to load OBJ files.
     */
    class OBJLoader {
    public:

        /**
         * Loads OBJ file contents into a collection of meshes.
         *
         * @param path The path to the file.
         *
         * @returns A collection of meshes.
         */
        static std::vector<StaticMesh> LoadObjFile( const std::string& path );
    };
}