#include "Transform.h"

namespace Epoch {

    Transform::Transform() {

    }

    Transform::Transform( const Transform& other ) {
        Position = other.Position;
        Rotation = other.Rotation;
        Scale = other.Scale;
    }

    Matrix4x4 Transform::GetTransformation() const {

        // SQT/SRT
        Matrix4x4 scaleMatrix, quatMatrix, translationMatrix;
        Matrix4x4::Scale( Scale, &scaleMatrix );
        Rotation.ToMatrix4x4( &quatMatrix );
        Matrix4x4::Translation( Position, &translationMatrix );

        return translationMatrix * quatMatrix * scaleMatrix;
    }
}