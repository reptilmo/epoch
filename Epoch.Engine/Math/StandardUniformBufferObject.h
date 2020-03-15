#pragma once

#include "Matrix4x4.h"

namespace Epoch {

    struct StandardUniformBufferObject {
        Matrix4x4 Model;
        Matrix4x4 View;
        Matrix4x4 Projection;
    };
}