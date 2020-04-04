#pragma once

#include <vector>

#include "../Types.h"

#include "Vertex3D.h"

namespace Epoch {

    struct MeshUploadData {
        std::vector<Vertex3D> Vertices;
        std::vector<U32> Indices;
    };

    struct MeshRendererReferenceData {
        U64 VertexHeapIndex;
        U64 IndexHeapIndex;
    };
}