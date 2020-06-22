#pragma once

#include "../Defines.h"
#include "../Types.h"
#include "../Memory/Memory.h"

namespace Epoch {

    struct Vector3;

    /*
     * A 4x4 matrix of floating-point values.
     */
    ALIGN( 16 ) class EPOCH_API Matrix4x4 {
    public:

        /**
         * Creates a new 4x4 matrix.
         */
        Matrix4x4();

        /**
         * Creates a new 4x4 matrix using the values from the provided matrix.
         *
         * @param other The matrix whose values to copy.
         */
        Matrix4x4( const Matrix4x4& other );

    public:

        /**
         * Transposes this matrix.
         *
         * @return A reference to this transposed matrix.
         */
        Matrix4x4 Transpose();

        /**
         * Multiplies this matrix by the one provided.
         *
         * @param m The matrix to multiply by.
         *
         * @return A copy of the multiplied matrix.
         */
        Matrix4x4 operator*( const Matrix4x4& b ) const;

        /**
         * Multiplies this matrix by the one provided.
         *
         * @param m The matrix to multiply by.
         *
         * @return A reference to this multiplied matrix.
         */
        Matrix4x4& operator*=( const Matrix4x4& b );

        /**
         * Sets the values of this matrix to those of the one provided.
         *
         * @param m The matrix whose values to use.
         *
         * @return A reference to this modified matrix.
         */
        Matrix4x4 operator=( const Matrix4x4& m );

        /*
         * Obtains the internal data element within this matrix by index.
         *
         * @param index The index to get.
         */
        float& operator[]( const int index );

        /** Returns the a const reference to the float data of this matrix in array form. */
        const float* Data() const { return _data; }

        /** Returns the a reference to the float data of this matrix in array form. */
        float* Data() { return _data; }

    public:

        /**
         * Creates and returns an identity matrix.
         *
         * @return Identity matrix.
         */
        static Matrix4x4 Identity();

        /**
         * Creates and returns an orthographic matrix using the supplied parameters.
         *
         * @param left The position of the left clipping plane.
         * @param right The position of the right clipping plane.
         * @param bottom The position of the bottom clipping plane.
         * @param top The position of the top clipping plane.
         * @param nearClip The position of the near clipping plane.
         * @param farClip The position of the far clipping plane.
         *
         * @return Orthographic matrix.
         */
        static Matrix4x4 Orthographic( const float left, const float right, const float bottom, const float top, const float nearClip, const float farClip );

        /**
         * Creates and returns a perspective matrix using the supplied parameters.
         *
         * @param fov The field of view in radians.
         * @param aspect The aspect ratio.
         * @param nearClip The position of the near clipping plane.
         * @param farClip The The position of the far clipping plane.
         *
         * @return Orthographic matrix.
         */
        static Matrix4x4 Perspective( const float fov, const float aspect, const float nearClip, const float farClip );

        /**
         * Creates and returns a lookAt matrix using the supplied parameters.
         *
         * @param position The position of the matrix.
         * @param target The target position of the matrix (look-at point).
         * @param up The the up vector.
         *
         * @return LookAt matrix.
         */
        static Matrix4x4 LookAt( const Vector3& position, const Vector3& target, const Vector3& up );

        /**
         * Creates and returns a translation matrix using the supplied parameters.
         *
         * @param position The position of the matrix.
         *
         * @return Translation matrix.
         */
        static void Translation( const Vector3& position, Matrix4x4* translationMatrix );

        /**
         * Creates and returns a Euler x-axis-angle rotation matrix using the supplied parameters.
         *
         * @param angleInRadians The x-axis angle in radians.
         *
         * @return Rotation matrix.
         */
        static Matrix4x4 RotationX( const float angleInRadians );

        /**
         * Creates and returns a Euler y-axis-angle rotation matrix using the supplied parameters.
         *
         * @param angleInRadians The y-axis angle in radians.
         *
         * @return Rotation matrix.
         */
        static Matrix4x4 RotationY( const float angleInRadians );

        /**
         * Creates and returns a Euler z-axis-angle rotation matrix using the supplied parameters.
         *
         * @param angleInRadians The z-axis angle in radians.
         *
         * @return Rotation matrix.
         */
        static Matrix4x4 RotationZ( const float angleInRadians );

        /**
         * Creates and returns a combined Euler x,- y- and z-axis-angle rotation matrix using the supplied parameters.
         *
         * @param xRadians The x-axis angle in radians.
         * @param yRadians The y-axis angle in radians.
         * @param zRadians The z-axis angle in radians.
         *
         * @return Rotation matrix.
         */
        static Matrix4x4 RotationXYZ( const float xRadians, const float yRadians, const float zRadians );

        /**
         * Creates and returns a uniform scale matrix using the supplied parameters.
         *
         * @param scale The scale to be applied.
         *
         * @return Scale matrix.
         */
        static void Scale( const Vector3& scale, Matrix4x4* scaleMatrix );

        /**
         * Creates and returns transposed version of the supplied matrix.
         *
         * @param m The matrix to transpose.
         *
         * @return A copy of the transposed matrix.
         */
        static Matrix4x4 Transposed( const Matrix4x4& m );

    private:
        ALIGN( 16 ) float _data[16];

        friend struct Quaternion;
    };

    FORCEINLINE Matrix4x4::Matrix4x4() {
        TMemory::MemZero( _data, ( sizeof( float ) * 16 ) );

        // Default to identity.
        _data[0] = 1.0f;
        _data[5] = 1.0f;
        _data[10] = 1.0f;
        _data[15] = 1.0f;
    }

    FORCEINLINE Matrix4x4::Matrix4x4( const Matrix4x4& other ) {
        /*for( U32 i = 0; i < 16; ++i ) {
            _data[i] = other._data[i];
        }*/
        TMemory::Memcpy( _data, other._data, sizeof( float ) * 16 );
    }

    FORCEINLINE Matrix4x4 Matrix4x4::Transpose() {
        *this = Matrix4x4::Transposed( *this );
        return *this;
    }

    FORCEINLINE Matrix4x4& Matrix4x4::operator*=( const Matrix4x4& b ) {

        *this = ( *this ) * b;
        return *this;
    }

    FORCEINLINE Matrix4x4 Matrix4x4::operator=( const Matrix4x4& m ) {
        for( U32 i = 0; i < 16; ++i ) {
            _data[i] = m._data[i];
        }
        //TMemory::Memcpy( _data, m._data, sizeof( float ) * 16 );
        return *this;
    }

    FORCEINLINE float& Matrix4x4::operator[]( const int index ) {
        //ASSERT_MSG( index >= 0 && index <= 15, "Matrix4x4[]: Index out of range" );
        return _data[index];
    }

    FORCEINLINE Matrix4x4 Matrix4x4::Identity() {
        Matrix4x4 m;
        m[0] = 1.0f;
        m[5] = 1.0f;
        m[10] = 1.0f;
        m[15] = 1.0f;
        return m;
    }

    FORCEINLINE Matrix4x4 Matrix4x4::operator*( const Matrix4x4& b ) const {
        Matrix4x4 dst;

        const F32* m1Ptr = b._data;
        const F32* m2Ptr = _data;
        F32* dstPtr = dst._data;

        for( I32 i = 0; i < 4; ++i ) {
            for( I32 j = 0; j < 4; ++j ) {
                *dstPtr =
                    m1Ptr[0] * m2Ptr[0 + j] +
                    m1Ptr[1] * m2Ptr[4 + j] +
                    m1Ptr[2] * m2Ptr[8 + j] +
                    m1Ptr[3] * m2Ptr[12 + j];
                dstPtr++;
            }
            m1Ptr += 4;
        }
        return dst;
    }
}