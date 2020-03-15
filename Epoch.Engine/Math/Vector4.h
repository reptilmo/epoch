#pragma once

#include "../Types.h"
#include "../Defines.h"

namespace Epoch {

    /**
     * A homogeneous 4-dimensional vector. 16-bit aligned.
     */
    ALIGN( 16 ) struct Vector4 {
    public:

        /** The X-component of this vector. */
        float X;

        /** The Y-component of this vector. */
        float Y;

        /** The Z-component of this vector. */
        float Z;

        /** The W-component of this vector. */
        float W;
    public:

        /**
         * Creates a new Vector4 with values defaulted to 0.
         */
        Vector4();

        /**
         * Creates a new Vector4 with values defaulted to 0.
         *
         * @param xyzw The value to set the X-, Y- Z- and W-components to.
         */
        Vector4( const float xyzw );

        /**
         * Creates a new Vector4 with values defaulted to 0.
         *
         * @param x The value to set the X-component to.
         * @param y The value to set the Y-component to.
         * @param z The value to set the Z-component to.
         * @param w The value to set the W-component to.
         */
        Vector4( const float x, const float y, const float z, const float w );

        /**
         * Sets the values of this vector.
         *
         * @param x The value to set the X-component of this vector to.
         * @param y The value to set the Y-component of this vector to.
         * @param z The value to set the Z-component of this vector to.
         * @param 2 The value to set the W-component of this vector to.
         */
        void Set( const float x, const float y, const float z, const float w );

        /**
         * Assigns the elements in this vector to the values of that passed in.
         *
         * @param v The other vector to obtain values from.
         *
         * @returns This updated vector.
         */
        Vector4 operator=( const Vector4& v );

        /**
         * Multiplies the elements in this vector by the scalar passed in.
         *
         * @param scalar The scalar to multiply by.
         *
         * @returns A copy of the multiplied vector.
         */
        Vector4 operator*( const float& scalar ) const;

        /**
         * Divides the elements in this vector by the scalar passed in.
         *
         * @param scalar The scalar to divide by.
         *
         * @returns A copy of the divided vector.
         */
        Vector4 operator/( const float& scalar ) const;

        /**
         * Multiplies the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to multiply by.
         *
         * @returns A copy of the multiplied vector.
         */
        Vector4 operator*( const Vector4& v ) const;

        /**
         * Divides the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to divide by.
         *
         * @returns A copy of the divided vector.
         */
        Vector4 operator/( const Vector4& v ) const;

        /**
         * Adds the values of the vector passed in to the elements in this vector.
         *
         * @param v The vector to add.
         *
         * @returns A copy of the added vector.
         */
        Vector4 operator+( const Vector4& v ) const;

        /**
         * Subtracts the values of the vector passed in from the elements in this vector.
         *
         * @param v The vector to subtract.
         *
         * @returns A copy of the subtracted vector.
         */
        Vector4 operator-( const Vector4& v ) const;

        /**
         * Multiplies the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to multiply by.
         *
         * @returns This multiplied vector.
         */
        Vector4 operator*=( const Vector4& v );

        /**
         * Divides the elements in this vector by the values of the vector passed in.
         *
         * @param v The vector to divide by.
         *
         * @returns This divided vector.
         */
        Vector4 operator/=( const Vector4& v );

        /**
         * Adds the values of the vector passed in to the elements in this vector.
         *
         * @param v The vector to add.
         *
         * @returns This added vector.
         */
        Vector4 operator+=( const Vector4& v );

        /**
         * Subtracts the values of the vector passed in from the elements in this vector.
         *
         * @param v The vector to subtract.
         *
         * @returns This subtracted vector.
         */
        Vector4 operator-=( const Vector4& v );

        /**
         * Tests this vector to the one passed in for equality.
         *
         * @param v The other vector to compare against.
         *
         * @return true if the same; otherwise false.
         */
        const bool operator==( const Vector4& v ) const;

        /**
         * Tests this vector to the one passed in for inequality.
         *
         * @param v The other vector to compare against.
         *
         * @return true if the different; otherwise false.
         */
        const bool operator!=( const Vector4& v ) const;

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
        static Vector4 Zero();

        /** Returns a vector whose components are all set to one. */
        static Vector4 One();

        /**
         * Extracts the values of a vector from the provided string.
         *
         * @param str The string from which to extract.
         *
         * @return The extracted vector.
         */
        static Vector4 FromString( const char* str );
    };
}