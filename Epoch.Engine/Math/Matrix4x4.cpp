#include <math.h>

#include "../Logger.h"
#include "../Defines.h"
#include "Vector3.h"
#include "Matrix4x4.h"

namespace Epoch {

    Matrix4x4::Matrix4x4() {

        // Identity matrix by default.
        _data[0] = 1.0f;
        _data[5] = 1.0f;
        _data[10] = 1.0f;
        _data[15] = 1.0f;
    }

    Matrix4x4::Matrix4x4( const Matrix4x4& other ) {
        for( U32 i = 0; i < 16; ++i ) {
            _data[i] = other._data[i];
        }
    }

    Matrix4x4 Matrix4x4::Transpose() {
        *this = Matrix4x4::Transposed( *this );
        return *this;
    }

    Matrix4x4 Matrix4x4::operator*( const Matrix4x4& b ) const {
        Matrix4x4 m = *this;
        return m *= b;
    }

    Matrix4x4 Matrix4x4::operator*=( const Matrix4x4& b ) {
        float b00 = b._data[0 * 4 + 0];
        float b01 = b._data[0 * 4 + 1];
        float b02 = b._data[0 * 4 + 2];
        float b03 = b._data[0 * 4 + 3];
        float b10 = b._data[1 * 4 + 0];
        float b11 = b._data[1 * 4 + 1];
        float b12 = b._data[1 * 4 + 2];
        float b13 = b._data[1 * 4 + 3];
        float b20 = b._data[2 * 4 + 0];
        float b21 = b._data[2 * 4 + 1];
        float b22 = b._data[2 * 4 + 2];
        float b23 = b._data[2 * 4 + 3];
        float b30 = b._data[3 * 4 + 0];
        float b31 = b._data[3 * 4 + 1];
        float b32 = b._data[3 * 4 + 2];
        float b33 = b._data[3 * 4 + 3];
        float a00 = _data[0 * 4 + 0];
        float a01 = _data[0 * 4 + 1];
        float a02 = _data[0 * 4 + 2];
        float a03 = _data[0 * 4 + 3];
        float a10 = _data[1 * 4 + 0];
        float a11 = _data[1 * 4 + 1];
        float a12 = _data[1 * 4 + 2];
        float a13 = _data[1 * 4 + 3];
        float a20 = _data[2 * 4 + 0];
        float a21 = _data[2 * 4 + 1];
        float a22 = _data[2 * 4 + 2];
        float a23 = _data[2 * 4 + 3];
        float a30 = _data[3 * 4 + 0];
        float a31 = _data[3 * 4 + 1];
        float a32 = _data[3 * 4 + 2];
        float a33 = _data[3 * 4 + 3];

        _data[0] = b00 * a00 + b01 * a10 + b02 * a20 + b03 * a30;
        _data[1] = b00 * a01 + b01 * a11 + b02 * a21 + b03 * a31;
        _data[2] = b00 * a02 + b01 * a12 + b02 * a22 + b03 * a32;
        _data[3] = b00 * a03 + b01 * a13 + b02 * a23 + b03 * a33;
        _data[4] = b10 * a00 + b11 * a10 + b12 * a20 + b13 * a30;
        _data[5] = b10 * a01 + b11 * a11 + b12 * a21 + b13 * a31;
        _data[6] = b10 * a02 + b11 * a12 + b12 * a22 + b13 * a32;
        _data[7] = b10 * a03 + b11 * a13 + b12 * a23 + b13 * a33;
        _data[8] = b20 * a00 + b21 * a10 + b22 * a20 + b23 * a30;
        _data[9] = b20 * a01 + b21 * a11 + b22 * a21 + b23 * a31;
        _data[10] = b20 * a02 + b21 * a12 + b22 * a22 + b23 * a32;
        _data[11] = b20 * a03 + b21 * a13 + b22 * a23 + b23 * a33;
        _data[12] = b30 * a00 + b31 * a10 + b32 * a20 + b33 * a30;
        _data[13] = b30 * a01 + b31 * a11 + b32 * a21 + b33 * a31;
        _data[14] = b30 * a02 + b31 * a12 + b32 * a22 + b33 * a32;
        _data[15] = b30 * a03 + b31 * a13 + b32 * a23 + b33 * a33;

        return *this;
    }

    Matrix4x4 Matrix4x4::operator=( const Matrix4x4& m ) {
        for( U32 i = 0; i < 16; ++i ) {
            _data[i] = m._data[i];
        }
        return *this;
    }

    float* Matrix4x4::operator[]( const int index ) {
        ASSERT_MSG( index >= 0 && index <= 15, "Matrix4x4[]: Index out of range" );
        return &_data[index];
    }

    Matrix4x4 Matrix4x4::Identity() {
        return Matrix4x4();
    }

    Matrix4x4 Matrix4x4::Orthographic( const float left, const float right, const float bottom, const float top, const float nearClip, const float farClip ) {
        Matrix4x4 m;

        float lr = 1.0f / ( left - right );
        float bt = 1.0f / ( bottom - top );
        float nf = 1.0f / ( nearClip - farClip );

        m._data[0] = -2.0f * lr;
        m._data[5] = -2.0f * bt;
        m._data[10] = 2.0f * nf;

        m._data[12] = ( left + right ) * lr;
        m._data[13] = ( top + bottom ) * bt;
        m._data[14] = ( farClip + nearClip ) * nf;

        return m;
    }

    Matrix4x4 Matrix4x4::Perspective( const float fov, const float aspect, const float nearClip, const float farClip ) {
        float f = 1.0f / tanf( fov / 2.0f );
        float rangeInv = 1.0f / ( nearClip - farClip );

        Matrix4x4 m;
        m._data[0] = f / aspect;
        m._data[5] = f;
        m._data[10] = ( nearClip + farClip ) * rangeInv;
        m._data[11] = -1.0f; // 0.0f?
        m._data[14] = nearClip * farClip * rangeInv * 2.0f;
        m._data[15] = 0.0f;

        return m;
    }

    Matrix4x4 Matrix4x4::LookAt( const Vector3& position, const Vector3& target, const Vector3& up ) {

        // Same as the DirectX implementation of this algorithm, with the exception that the zAxis is 
        // determined by reversing the subtraction of position and target.
        Vector3 zAxis = Vector3::Normalized( position - target );
        Vector3 xAxis = Vector3::Normalized( Vector3::Cross( up, zAxis ) );
        Vector3 yAxis = Vector3::Cross( zAxis, xAxis );

        Matrix4x4 m;
        m._data[0] = xAxis.X;
        m._data[1] = yAxis.X;
        m._data[2] = zAxis.X;
        m._data[3] = 0.0f;
        m._data[4] = xAxis.Y;
        m._data[5] = yAxis.Y;
        m._data[6] = zAxis.Y;
        m._data[7] = 0.0f;
        m._data[8] = xAxis.Z;
        m._data[9] = yAxis.Z;
        m._data[10] = zAxis.Z;
        m._data[11] = 0.0f;
        m._data[12] = -xAxis.Dot( position );
        m._data[13] = -yAxis.Dot( position );
        m._data[14] = -zAxis.Dot( position );
        m._data[15] = 1.0f;
        return m;
    }

    Matrix4x4 Matrix4x4::Translation( const Vector3& position ) {
        Matrix4x4 m;
        m._data[13] = position.Y;
        m._data[12] = position.X;
        m._data[14] = position.Z;
        return m;
    }

    Matrix4x4 Matrix4x4::RotationX( const float angleInRadians ) {
        Matrix4x4 m;
        float c = cosf( angleInRadians );
        float s = sinf( angleInRadians );

        m._data[5] = c;
        m._data[6] = s;
        m._data[9] = -s;
        m._data[10] = c;

        return m;
    }

    Matrix4x4 Matrix4x4::RotationY( const float angleInRadians ) {
        Matrix4x4 m;
        float c = cosf( angleInRadians );
        float s = sinf( angleInRadians );

        m._data[0] = c;
        m._data[2] = -s;
        m._data[8] = s;
        m._data[10] = c;

        return m;
    }

    Matrix4x4 Matrix4x4::RotationZ( const float angleInRadians ) {
        Matrix4x4 m;
        float c = cosf( angleInRadians );
        float s = sinf( angleInRadians );

        m._data[0] = c;
        m._data[1] = s;
        m._data[4] = -s;
        m._data[5] = c;

        return m;
    }

    Matrix4x4 Matrix4x4::RotationXYZ( const float xRadians, const float yRadians, const float zRadians ) {
        Matrix4x4 rx = Matrix4x4::RotationX( xRadians );
        Matrix4x4 ry = Matrix4x4::RotationY( yRadians );
        Matrix4x4 rz = Matrix4x4::RotationZ( zRadians );

        // ZYX 
        return rz * ry * rx;
    }

    Matrix4x4 Matrix4x4::Scale( const Vector3& Scale ) {
        Matrix4x4 m;
        m._data[0] = Scale.X;
        m._data[5] = Scale.Y;
        m._data[10] = Scale.Z;
        return m;
    }

    Matrix4x4 Matrix4x4::Transposed( const Matrix4x4& other ) {
        Matrix4x4 m;
        m._data[0] = other._data[0];
        m._data[1] = other._data[4];
        m._data[2] = other._data[8];
        m._data[3] = other._data[12];
        m._data[4] = other._data[1];
        m._data[5] = other._data[5];
        m._data[6] = other._data[9];
        m._data[7] = other._data[13];
        m._data[8] = other._data[2];
        m._data[9] = other._data[9];
        m._data[10] = other._data[10];
        m._data[11] = other._data[14];
        m._data[12] = other._data[3];
        m._data[13] = other._data[7];
        m._data[14] = other._data[11];
        m._data[15] = other._data[15];
        return m;
    }
}