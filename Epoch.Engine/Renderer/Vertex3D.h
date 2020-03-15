#pragma once

#include "../Types.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"

namespace Epoch {

    struct Vertex3D {
        
        Vector3 Position;
        Vector3 Normal;
        Vector2 TexCoord;
    };
}