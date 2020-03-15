#include "Transform.h"

namespace Epoch {

    Transform::Transform() {

    }

    Transform::Transform( const Transform& other ) {
        position = other.position;
        rotation = other.rotation;
        scale = other.scale;
    }

    Matrix4x4 Transform::GetTransformation() const {

        // SQT/SRT
        Matrix4x4 s = Matrix4x4::Scale( scale );
        Matrix4x4 q = rotation.ToMatrix4x4();
        Matrix4x4 t = Matrix4x4::Translation( position );

        return t * q * s;
    }
}