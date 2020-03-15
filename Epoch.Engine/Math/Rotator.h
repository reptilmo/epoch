#pragma once

namespace Epoch {

    struct Quaternion;
    struct Vector3;

    /**
     * A container to store rotation information. Rotation values are stored in degrees.
     */
    struct Rotator {
    public:

        /**
         * Rotation about the X (left) axis in degrees. (0 = straight, positive looks Up and negative looks Down).
         */
        float Pitch;

        /**
         * Rotation about the Y (up) axis in degrees. (0 = East, Positive heads North and negative heads South).
         */
        float Yaw;

        /**
         * Rotation about the Z (forward) axis in degrees. (0 = straight, Positive tilts clockwise and negative tilts counter-clockwise).
         */
        float Roll;

    public:

        /**
         * Creates a new Rotator without initialization.
         */
        Rotator();

        /**
         * Creates a new Rotator with the values set to those supplied initialization.
         *
         * @param pitch Rotation about the X (left) axis in degrees.
         * @param yaw Rotation about the Y (up) axis in degrees.
         * @param roll Rotation about the Z (forward) axis in degrees.
         */
        Rotator( const float pitch, const float yaw, const float roll );

        /**
         * Converts this rotator to a Quaternion.
         *
         * @return The quaternion created from this rotator.
         */
        Quaternion ToQuaternion() const;

        /**
         * Converts this rotator to a Euler, represented by a Vector3.
         *
         * @return The Euler created from this rotator.
         */
        Vector3 ToEuler() const;

        /**
         * Gets a specific component of this rotator.
         *
         * @param index The index of the component to get.
         *
         * @return A reference to the component.
         */
        float& operator[]( const int index );

        /**
         * Gets a specific component of this rotator.
         *
         * @param index The index of the component to get.
         *
         * @return A copy of the component.
         */
        const float& operator[]( const int index ) const;

    public:

        /**
         * Creates a new Rotator from the supplied quaternion.
         *
         * @param q The quaternion to create the rotator from.
         *
         * @return The rotator.
         */
        static Rotator FromQuaternion( const Quaternion& q );

        /**
         * Creates a new Rotator with all elements initialized to 0.
         *
         * @return The rotator.
         */
        static Rotator Zero();
    };
}