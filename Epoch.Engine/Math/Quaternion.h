#pragma once

#include "../Defines.h"
#include "Vector3.h"

namespace Epoch {

    struct Rotator;
    struct Vector4;
    class Matrix4x4;

    /*
     * Floating point quaternion that represents a rotation about an axis in 3D space.
     */
    struct EPOCH_API Quaternion {
    public:

        /** The X-component. */
        float X;

        /** The Y-component. */
        float Y;

        /** The Z-component. */
        float Z;

        /** The W-component. */
        float W;

    public:

        /**
         * Creates a new quaternion using default values of 0.
         */
        Quaternion();

        /**
         * Creates a new quaternion using the values of the quaternion passed in.
         *
         * @param other The quaternion whose values to use.
         */
        Quaternion( const Quaternion& other );

        /**
         * Creates a new quaternion using the values passed in.
         *
         * @param x The x-component.
         * @param y The y-component.
         * @param z The z-component.
         * @param w The w-component.
         */
        Quaternion( const float x, const float y, const float z, const float w );

        /**
         * Obtains the inverse of this quaternion. (Conjugate / Normal)
         *
         * @return The inverse of this quaternion as a copy.
         */
        Quaternion Inverse() const;

        /**
         * Obtains the conjugate of this quaternion. (-x, -y, -z, w)
         *
         * @return The conjugate of this quaternion as a copy.
         */
        Quaternion Conjugate() const;

        /**
         * Calculates the product of this quaternion and the one passed in.
         *
         * @param q The quaternion to multiply this one by.
         *
         * @return The product as a copy.
         */
        Quaternion Product( const Quaternion& q ) const;

        /**
         * Calculates the dot product of this quaternion and the one passed in.
         *
         * @param q The quaternion to calculate against.
         *
         * @return The dot product as a copy.
         */
        const float DotProduct( const Quaternion& q ) const;

        /**
         * Returns the normal of this quaternion.
         */
        float Normal() const;

        /**
         * Normalizes this quaternion.
         */
        void Normalize();

        /*
         * Gets a specific component of this quaternion.
         *
         * @param index The index of the component to get.
         *
         * @return A reference to the component.
         */
        float& operator[]( const int index );

        /*
         * Gets a specific component of this quaternion.
         *
         * @param index The index of the component to get.
         *
         * @return A copy of the component.
         */
        float operator[]( const int index ) const;

        /**
         * Multiplies the elements in this quaternion by the values of the quaternion passed in.
         *
         * @param q The quaternion to multiply by.
         *
         * @returns A copy of the multiplied quaternion.
         */
        Quaternion operator* ( const Quaternion& q ) const;

        /**
         * Multiplies the elements in this quaternion by the scalar passed in.
         *
         * @param s The scalar to multiply by.
         *
         * @returns A copy of the multiplied quaternion.
         */
        Quaternion operator* ( const float s ) const;

        /**
         * Adds the values of the quaternion passed in to the elements in this quaternion.
         *
         * @param q The quaternion to add.
         *
         * @returns A copy of the added quaternion.
         */
        Quaternion operator+ ( const Quaternion& q ) const;

        /**
         * Subtracts the values of the quaternion passed in from the elements in this quaternion.
         *
         * @param q The quaternion to subtract.
         *
         * @returns A copy of the subtracted quaternion.
         */
        Quaternion operator- ( const Quaternion& q ) const;

        /**
         * Divides the elements in this quaternion by the values of the quaternion passed in.
         *
         * @param q The quaternion to divide by.
         *
         * @returns A copy of this divided quaternion.
         */
        Quaternion operator/ ( const float s ) const;

        /**
         * Multiplies the elements in this quaternion by the values of the quaternion passed in.
         *
         * @param q The quaternion to multiply by.
         *
         * @returns A reference to this multiplied quaternion.
         */
        Quaternion operator*= ( const Quaternion& q );

        /*
         * Gets the negative of this quaternion.
         *
         * @returns A copy of this divided quaternion.
         */
        Quaternion operator- () const;

        /*
         * Calculates a 4x4 rotation matrix from this quaternion.
         *
         * @returns A new rotation matrix.
         */
        void ToMatrix4x4( Matrix4x4* quatMatrix ) const;

        /**
         * Calculates a rotation matrix based on this quaternion and the passed in center point.
         *
         * @param center The center point.
         *
         * @returns A new rotation matrix.
         */
        Matrix4x4 Rotation( const Vector3& center ) const;

        /*
         * Creates a new Vector4 from this quaternion.
         *
         * @returns A new Vector4.
         */
        Vector4 ToVector4() const;

        /*
         * Creates a new Rotator from this quaternion.
         *
         * @returns A new Rotator.
         */
        Rotator ToRotator() const;

        /**
         * Gets a normalized copy of the passed in quaternion.
         *
         * @param q The quaternion to be normalized.
         *
         * @returns A normalized copy of the quaternion.
         */
        static Quaternion Normalized( const Quaternion& q );

        /**
         * Gets a new quaternion based on the passed in Euler angles.
         *
         * @param rotator The Rotaotr angles to be used.
         *
         * @returns A new quaternion.
         */
        static Quaternion FromRotator( const Rotator& rotator );

        /**
         * Gets a new quaternion based on the passed in axis and angle.
         *
         * @param axis The axis to rotate around.
         * @param angle The angle in radians to rotate.
         * @param normalize Indicates if the returned quaternion should be normalized.
         *
         * @returns A new quaternion.
         */
        static Quaternion FromAxisAngle( const Vector3& axis, const float angle, bool normalize = true );

        /**
         * Gets the spherical linear interpolation between the two provided quaternions based on the percentage passed.
         *
         * @param q0 The first quaternion.
         * @param q0 The second quaternion.
         * @param percentage The percentage between q0 and q1.
         *
         * @returns A new quaternion.
         */
        static Quaternion Slerp( const Quaternion& q0, const Quaternion& q1, float percentage );

        /**
         * Extracts the values of a quaternion from the provided string.
         *
         * @param str The string from which to extract.
         *
         * @return The extracted quaternion.
         */
        static Quaternion FromString( const char* str );
    private:
        Vector3 _axis;
    };
}