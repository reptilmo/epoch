#pragma once

#include <string>
#include <vector>

#include "../AssetData.h"
#include "StaticMesh.h"

namespace Epoch {

    /**
     * Represents raw static mesh asset data, used to load a static mesh.
     */
    class StaticMeshAssetData : public AssetData {
    public:

        StaticMeshAssetData( const std::string path, const std::string name, std::vector<StaticMeshData> data ) :AssetData( path, name, AssetType::StaticMesh ) {
            _data = data;
        }

        const std::vector<StaticMeshData>& GetData() const { return _data; }

    protected:
        std::vector<StaticMeshData> _data;
    };
}