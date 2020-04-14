#pragma once

#include "../Math/Matrix4x4.h"

namespace Epoch {

    /**
     * A uniform object which is used for commonly-used, rarely-updated global data, such as view/projection matrix. Must be 256 bytes total.
     */
    struct GlobalUniformObject {
        Matrix4x4 View; // 64 bytes
        Matrix4x4 Projection; // 64 bytes

        Matrix4x4 Reserved1; // 64 bytes, reserved for future use
        Matrix4x4 Reserved2; // 64 bytes, reserved for future use
    };

    /**
     * A uniform object which is used for commonly-used, commonly-updated, per-object data, such as the model matrix or material data. 
     * For use with unlit materials. Must be 256 bytes total.
     */
    struct UnlitUniformObject {
        Matrix4x4 Model; // 64 bytes

        Matrix4x4 Reserved1; // 64 bytes, reserved for future use
        Matrix4x4 Reserved2; // 64 bytes, reserved for future use
        Matrix4x4 Reserved3; // 64 bytes, reserved for future use
    };
}