#pragma once

#include <vector>

#include "../Types.h"
#include "../Math/Transform.h"

#include "Vertex3D.h"

namespace Epoch {

    class BaseMaterial;

    struct MeshUploadData {
        std::vector<Vertex3D> Vertices;
        std::vector<U32> Indices;
    };

    struct MeshRendererReferenceData {
        U64 VertexHeapIndex;
        U64 IndexHeapIndex;

        BaseMaterial* Material;

        Transform* Transform;
    };
}