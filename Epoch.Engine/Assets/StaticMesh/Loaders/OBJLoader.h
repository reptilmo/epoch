#pragma once

#include <vector>
#include <string>
#include <functional>

#include "../StaticMesh.h"

namespace Epoch {

    typedef std::function<void( std::vector<StaticMeshData> )> StaticMeshLoadedCallback;

    /**
     * Used to load OBJ files.
     */
    class OBJLoader {
    public:

        /**
         * Loads OBJ file contents into a collection of meshes. This is done asynchronously and returns immediately.
         * The an AssetLoaded event is sent on the next frame after the load completes.
         *
         * @param path The path to the file.
         *
         * @returns A collection of meshes.
         */
        static void LoadObjFile( const std::string& name, const std::string& path );

    private:
        static void loadOnThread( const std::string& name, const std::string& path );
    };
}