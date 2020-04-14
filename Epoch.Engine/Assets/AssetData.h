#pragma once

#include <vector>

#include "../String/TString.h"

namespace Epoch {

    enum class AssetType {
        Image,
        StaticMesh,

        Custom
    };

    class AssetData {
    public:

        AssetData( const TString& path, const TString& name, const AssetType type ) {
            _path = path;
            _name = name;
            _type = type;
        }

        const TString& GetName() const noexcept { return _name; }
        const TString& GetPath() const noexcept { return _path; }
        const AssetType GetType() const noexcept { return _type; }

    protected:
        TString _name;
        TString _path;
        AssetType _type;
    };
}