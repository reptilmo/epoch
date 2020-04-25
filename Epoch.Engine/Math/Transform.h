#pragma once

#include "../Defines.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

namespace Epoch {

    /*
     * Represents the transform of an object or entity in the world.
     */
    class EPOCH_API Transform {
    public:

        /** The position of this transform. */
        Vector3 Position;

        /** The rotation of this transform. */
        Quaternion Rotation;

        /** The scale of this transform. */
        Vector3 Scale = Vector3::One();
    public:

        /**
         * Creates a new transform with default values.
         */
        Transform();

        /**
         * Creates a new transform using the values of the one provided.
         *
         * @param other The transform whose values to copy.
         */
        Transform( const Transform& other );

        /**
         * Creates and returns a transformation matrix based on this transform.
         *
         * @return A new transformation matrix.
         */
        Matrix4x4 GetTransformation() const;

    private:
        void onPositionChanged( const Vector3& oldPosition, const Vector3& newPosition );
    };
}