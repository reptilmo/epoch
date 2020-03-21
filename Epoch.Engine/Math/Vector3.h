#pragma once

#include <xhash>

#include "../Types.h"

namespace Epoch {

    /*
     * Represents a 3-dimensional vector in space which includes X, Y and Z components.
     */
    struct Vector3 {
    public:

        /** The X-component of this vector. */
        float X;

        /** The Y-component of this vector. */
        float Y;

        /** The Z-component of this vector. */
        float Z;

    public:

        /**
         * Creates a new Vector3 with values defaulted to 0.
         */
        Vector3();

        /**
         * Creates a new Vector3 with values defaulted to 0.
         *
         * @param xyz The value to set the X-, Y- and Z-components to.
         */
        Vector3( const float xyz );

        /**
         * Creates a new Vector3 with values copied from the one provided.
         *
         * @param other The vector to copy from.
         */
        Vector3( const Vector3& other );

        /**
         * Creates a new Vector3 with values defaulted to 0.
         *
         * @param x The value to set the X-component to.
         * @param y The value to set the Y-component to.
         * @param z The value to set the Z-component to.
         */
        Vector3( const float x, const float y, const float z );

    public:

        /**
         * Gets the length (magnitude) of this vector.
         *
         * @return Vector length.
         */
        float Length() const;

        /**
         * Gets the squared length (magnitude) of this vector.
         *
         * @return Squared vector length.
         */
        float LengthSquared() const;

        /**
         * Normalizes this vector.
         *
         * @return Normalized vector (this).
         */
        Vector3 Normalize();

        /**
         * Calculates the cross-product between this and the vector passed in.
         *
         * @param other The other vector to calculate against.
         *
         * @return The calculated cross-product vector.
         */
        Vector3 Cross( const Vector3& other );

        /**
         * Calculates the dot product of this and the vector passed in.
         *
         * @param other The other vector to calculate against.
         * @return The calculated dot product.
         */
        float Dot( const Vector3& other ) const;

        /**
         * Sets the values of this vector.
         *
         * @param x The value to set the X-component of this vector to.
         * @param y The value to set the Y-component of this vector to.
         * @param z The value to set the Z-component of this vector to.
         */
        void Set( const float x, const float y, const float z );

        /**
         * Outputs this vector to a string, separated by a space.
         * Example: "0.1 0.2 0.3"
         */
        const char* ToString() const;

        /**
         * Compares this vector to the one passed in, given the tolerance in the provided error limit.
         *
         * @param v The other vector to compare against.
         * @param tolerance The error limit to be used for comparison.
         *
         * @return true if the same; otherwise false.
         */
        const bool Compare( const Vector3& v, const float tolerance ) const;

        /**
         * Assigns the elements in this vector to the values of that passed in.
         *
         * @param v The other vector to obtain values from.
         *
         * @returns This updated vector.
         */
        Vector3 operator=( const Vector3& v );

        /**
         * Multiplies the elements in this vector by the scalar passed in.
         *
         * @param scalar The scalar to multiply by.
         *
         * @returns A copy of the multiplied vector.
         */
        Vector3 operator*( const float& scalar ) const;

        /**
         * Divides the elements in this vector by the scalar passed in.
         *
         * @param scalar The scalar to divide by.
         *
         * @returns A copy of the divided vector.
         */
        Vector3 operator/( const float& scalar ) const;

        /**
         * Multiplies the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to multiply by.
         *
         * @returns A copy of the multiplied vector.
         */
        Vector3 operator*( const Vector3& v ) const;

        /**
         * Divides the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to divide by.
         *
         * @returns A copy of the divided vector.
         */
        Vector3 operator/( const Vector3& v ) const;

        /**
         * Adds the values of the vector passed in to the elements in this vector.
         *
         * @param v The vector to add.
         *
         * @returns A copy of the added vector.
         */
        Vector3 operator+( const Vector3& v ) const;

        /**
         * Subtracts the values of the vector passed in from the elements in this vector.
         *
         * @param v The vector to subtract.
         *
         * @returns A copy of the subtracted vector.
         */
        Vector3 operator-( const Vector3& v ) const;

        /**
         * Multiplies the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to multiply by.
         *
         * @returns This multiplied vector.
         */
        Vector3 operator*=( const Vector3& v );

        /**
         * Divides the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to divide by.
         *
         * @returns A copy of this divided vector.
         */
        Vector3 operator/=( const Vector3& v );

        /**
         * Adds the values of the vector passed in to the elements in this vector.
         *
         * @param v The vector to add.
         *
         * @returns This added vector.
         */
        Vector3 operator+=( const Vector3& v );

        /**
         * Subtracts the values of the vector passed in from the elements in this vector.
         *
         * @param v The vector to subtract.
         *
         * @returns This subtracted vector.
         */
        Vector3 operator-=( const Vector3& v );

        /**
         * Tests this vector to the one passed in for equality.
         *
         * @param v The other vector to compare against.
         *
         * @return true if the same; otherwise false.
         */
        const bool operator==( const Vector3& v ) const;

        /**
         * Tests this vector to the one passed in for inequality.
         *
         * @param v The other vector to compare against.
         *
         * @return true if the different; otherwise false.
         */
        const bool operator!=( const Vector3& v ) const;

        /**
         * Gets a specific component of this vector.
         *
         * @param index The index of the component to get.
         *
         * @return A reference to the component.
         */
        float& operator[]( I32 index );

        /**
         * Gets a specific component of this vector.
         *
         * @param index The index of the component to get.
         *
         * @return A copy of the component.
         */
        float operator[]( I32 index ) const;

    public:

        /** Returns a vector whose components are all set to zero. */
        static Vector3 Zero();

        /** Returns a vector whose components are all set to one. */
        static Vector3 One();

        /** Epoch's up direction (0,1,0). */
        static Vector3 Up();

        /** Epoch's down direction (0,-1,0). */
        static Vector3 Down();

        /** Epoch's left direction (-1,0,0). */
        static Vector3 Left();

        /** Epoch's right direction (1,0,0). */
        static Vector3 Right();

        /** Epoch's forward direction (0,0,1). */
        static Vector3 Forward();

        /** Epoch's backward direction (0,0,-1). */
        static Vector3 Backward();

        /**
         * Returns the distance between the two provided vectors.
         *
         * @param a The first vector.
         * @param a The second vector.
         *
         * @return The distance.
         */
        static float Distance( const Vector3& a, const Vector3& b );

        /**
         * Returns a normalized version of the supplied vector.
         *
         * @param v The vector to be normalized.
         *
         * @return The normalized vector.
         */
        static Vector3 Normalized( const Vector3& v );

        /**
         * Calculates the cross product of the two provided vectors.
         *
         * @param a The first vector.
         * @param b The second vector.
         *
         * @return The cross product.
         */
        static Vector3 Cross( const Vector3& a, const Vector3& b );

        /**
         * Extracts the values of a vector from the provided string.
         *
         * @param str The string from which to extract.
         *
         * @return The extracted vector.
         */
        static Vector3 FromString( const char* str );
    };
}

namespace std {

    // Required for used in unordered_map
    template<> struct std::hash<Epoch::Vector3> {
        size_t operator() ( Epoch::Vector3 const& vertex ) const {
            return (
                ( std::hash<F32>()( vertex.X ) ^
                ( std::hash<F32>()( vertex.Y ) << 1 ) ) >> 1
                ) ^
                ( std::hash<F32>()( vertex.Z ) << 1 );
        }
    };
}