#pragma once

#include <string>
#include <vector>

namespace Epoch {

    enum class AssetType {
        Image,
        StaticMesh,

        Custom
    };

    class AssetData {
    public:

        AssetData( const std::string path, const std::string name, const AssetType type ) {
            _path = path;
            _name = name;
            _type = type;
        }

        const std::string& GetName() const { return _name; }
        const std::string& GetPath() const { return _path; }
        const AssetType GetType() const { return _type; }

    protected:
        std::string _name;
        std::string _path;
        AssetType _type;
    };
}