#pragma once

#include <xhash>

#include "../Types.h"

namespace Epoch {

    /*
     * Represents a 3-dimensional vector in space which includes X, Y and Z components.
     */
    struct Vector2 {
    public:

        /** The X-component of this vector. */
        float X;

        /** The Y-component of this vector. */
        float Y;

    public:

        /**
         * Creates a new Vector2 with values defaulted to 0.
         */
        Vector2();

        /**
         * Creates a new Vector2 with values defaulted to 0.
         *
         * @param xy The value to set the X- and Y-components to.
         */
        Vector2( const float xy );

        /**
         * Creates a new Vector2 with values defaulted to 0.
         *
         * @param x The value to set the X-component to.
         * @param y The value to set the Y-component to.
         */
        Vector2( const float x, const float y );

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
        Vector2 Normalize();

        /**
         * Sets the values of this vector.
         *
         * @param x The value to set the X-component of this vector to.
         * @param y The value to set the Y-component of this vector to.
         */
        void Set( const float x, const float y );

        /**
         * Compares this vector to the one passed in, given the tolerance in the provided error limit.
         *
         * @param v The other vector to compare against.
         * @param tolerance The error limit to be used for comparison.
         *
         * @return true if the same; otherwise false.
         */
        const bool Compare( const Vector2& v, const float tolerance ) const;

        /**
         * Assigns the elements in this vector to the values of that passed in.
         *
         * @param v The other vector to obtain values from.
         *
         * @returns This updated vector.
         */
        Vector2 operator=( const Vector2& v );

        /**
         * Multiplies the elements in this vector by the scalar passed in.
         *
         * @param scalar The scalar to multiply by.
         *
         * @returns A copy of the multiplied vector.
         */
        Vector2 operator*( const float& scalar ) const;

        /**
         * Divides the elements in this vector by the scalar passed in.
         *
         * @param scalar The scalar to divide by.
         *
         * @returns A copy of the divided vector.
         */
        Vector2 operator/( const float& scalar ) const;

        /**
         * Multiplies the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to multiply by.
         *
         * @returns A copy of the multiplied vector.
         */
        Vector2 operator*( const Vector2& v ) const;

        /**
         * Divides the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to divide by.
         *
         * @returns A copy of the divided vector.
         */
        Vector2 operator/( const Vector2& v ) const;

        /**
         * Adds the values of the vector passed in to the elements in this vector.
         *
         * @param v The vector to add.
         *
         * @returns A copy of the added vector.
         */
        Vector2 operator+( const Vector2& v ) const;

        /**
         * Subtracts the values of the vector passed in from the elements in this vector.
         *
         * @param v The vector to subtract.
         *
         * @returns A copy of the subtracted vector.
         */
        Vector2 operator-( const Vector2& v ) const;

        /**
         * Multiplies the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to multiply by.
         *
         * @returns This multiplied vector.
         */
        Vector2 operator*=( const Vector2& v );

        /**
         * Divides the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to divide by.
         *
         * @returns This divided vector.
         */
        Vector2 operator/=( const Vector2& v );

        /**
         * Adds the values of the vector passed in to the elements in this vector.
         *
         * @param v The vector to add.
         *
         * @returns This added vector.
         */
        Vector2 operator+=( const Vector2& v );

        /**
         * Subtracts the values of the vector passed in from the elements in this vector.
         *
         * @param v The vector to subtract.
         *
         * @returns This subtracted vector.
         */
        Vector2 operator-=( const Vector2& v );

        /**
         * Tests this vector to the one passed in for equality.
         *
         * @param v The other vector to compare against.
         *
         * @return true if the same; otherwise false.
         */
        const bool operator==( const Vector2& v ) const;

        /**
         * Tests this vector to the one passed in for inequality.
         *
         * @param v The other vector to compare against.
         *
         * @return true if the different; otherwise false.
         */
        const bool operator!=( const Vector2& v ) const;

        /*
         * Gets a specific component of this vector.
         *
         * @param index The index of the component to get.
         *
         * @return A reference to the component.
         */
        float& operator[]( I32 index );

        /*
         * Gets a specific component of this vector.
         *
         * @param index The index of the component to get.
         *
         * @return A copy of the component.
         */
        float operator[]( I32 index ) const;

    public:

        /** Returns a vector whose components are all set to zero. */
        static Vector2 Zero();

        /** Returns a vector whose components are all set to one. */
        static Vector2 One();

        /**
         * Returns the distance between the two provided vectors.
         *
         * @param a The first vector.
         * @param a The second vector.
         *
         * @return The distance.
         */
        static const float Distance( Vector2& a, Vector2& b );

        /**
         * Returns a normalized version of the supplied vector.
         *
         * @param v The vector to be normalized.
         *
         * @return The normalized vector.
         */
        static Vector2 Normalized( const Vector2& v );

        /**
         * Extracts the values of a vector from the provided string.
         *
         * @param str The string from which to extract.
         *
         * @return The extracted vector.
         */
        static Vector2 FromString( const char* str );
    };
}

namespace std {

    // Required for used in unordered_map
    template<> struct hash<Epoch::Vector2> {
        size_t operator() ( Epoch::Vector2 const& vertex ) const {

            // Reference: https://en.cppreference.com/w/cpp/utility/hash
            return hash<F32>()( vertex.X ) ^ ( hash<F32>()( vertex.Y ) << 1 );
        }
    };
}