#pragma once

#include <vector>

#include "../../Types.h"
#include "../../Renderer/Vertex3D.h"

namespace Epoch {


    struct StaticMesh {
        std::string Name;
        std::vector<Epoch::Vertex3D> Vertices;
        std::vector<U32> Indices;

        // TODO: Material info
    };
}