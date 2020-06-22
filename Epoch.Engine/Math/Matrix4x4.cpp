#include <math.h>

#include "../Logger.h"
#include "../Defines.h"
#include "Vector3.h"
#include "../Memory/Memory.h"
#include "Matrix4x4.h"

namespace Epoch {

    

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
        //Vector3 zAxis = Vector3::Normalized( target - position );
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

    void Matrix4x4::Translation( const Vector3& position, Matrix4x4* translationMatrix ) {
        translationMatrix->_data[13] = position.Y;
        translationMatrix->_data[12] = position.X;
        translationMatrix->_data[14] = position.Z;
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

    void Matrix4x4::Scale( const Vector3& Scale, Matrix4x4* scaleMatrix ) {
        scaleMatrix->_data[0] = Scale.X;
        scaleMatrix->_data[5] = Scale.Y;
        scaleMatrix->_data[10] = Scale.Z;
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