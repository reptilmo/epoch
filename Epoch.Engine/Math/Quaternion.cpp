#include <math.h>

#include "../Logger.h"
#include "../Defines.h"
#include "../String/StringUtilities.h"

#include "TMath.h"
#include "Rotator.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include "Quaternion.h"

namespace Epoch {

    Quaternion::Quaternion() {
        X = Y = Z = 0.0f;
        W = 1.0f;
    }

    Quaternion::Quaternion( const Quaternion& other ) {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
        W = other.W;
    }

    Quaternion::Quaternion( const float x, const float y, const float z, const float w ) {
        X = x;
        Y = y;
        Z = z;
        W = w;
    }

    Quaternion Quaternion::Conjugate() const {
        return Quaternion( -X, -Y, -Z, W );
    }

    Quaternion Quaternion::Product( const Quaternion& q ) const {
        return Quaternion(
            Y * q.Z - Z * q.Y + X * q.W + W * X,
            Z * q.X - X * q.Z + Y * q.W + W * Y,
            X * q.Y - Y * q.X + Z * q.W + W * Z,
            W * q.W - X * q.X - Y * q.Y - Z * Z
        );
    }

    const float Quaternion::DotProduct( const Quaternion& q ) const {
        return X * q.X +
            Y * q.Y +
            Z * q.Z +
            W * q.W;
    }

    float Quaternion::Normal() const {
        return sqrtf(
            X * X +
            Y * Y +
            Z * Z +
            W * W
        );
    }

    void Quaternion::Normalize() {
        float normal = Normal();
        X /= normal;
        Y /= normal;
        Z /= normal;
        W /= normal;
    }

    // General inverse. If using a unit Quaternion, use Conjugate() instead.
    Quaternion Quaternion::Inverse() const {
        return Conjugate() / Normal();
    }

    float& Quaternion::operator[]( const int index ) {
        ASSERT_MSG( index < 4 && index >= 0, "Quaternion 3 index out of bounds" );
        return ( &X )[index];
    }

    float Quaternion::operator[]( const int index ) const {
        ASSERT_MSG( index < 4 && index >= 0, "Quaternion 3 index out of bounds" );
        return ( &X )[index];
    }

    Quaternion Quaternion::operator* ( const Quaternion& q ) const {
        return Quaternion(
            W * q.X + X * q.W + Y * q.Z - Z * q.Y,
            W * q.Y + Y * q.W + Z * q.X - X * q.Z,
            W * q.Z + Z * q.W + X * q.Y - Y * q.X,
            W * q.W - X * q.X - Y * q.Y - Z * q.Z
        );
    }

    Quaternion Quaternion::operator* ( const float s ) const {
        return Quaternion( X * s, Y * s, Z * s, W * s );
    }

    Quaternion Quaternion::operator+ ( const Quaternion& q ) const {
        return Quaternion( X + q.X, Y + q.Y, Z + q.Z, W + q.W );
    }

    Quaternion Quaternion::operator- ( const Quaternion& q ) const {
        return Quaternion( X - q.X, Y - q.Y, Z - q.Z, W - q.W );
    }

    Quaternion Quaternion::operator*= ( const Quaternion& q ) {
        *this = ( *this ) * q;
        return *this;
    }

    Quaternion Quaternion::operator- () const {
        return Quaternion( -X, -Y, -Z, -W );
    }

    Quaternion Quaternion::operator/ ( const float s ) const {
        if( s == 0 ) {
            // BAD!
            Logger::Warn( "Quaternion division by 0!" );
        }
        return Quaternion( X / s, Y / s, Z / s, W / s );
    }

    Matrix4x4 Quaternion::ToMatrix4x4() const {

        // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix
        Quaternion q = *this;
        q.Normalize();

        Matrix4x4 m;
        ( *m[0] ) = 1.0f - 2.0f * Y * Y - 2.0f * Z * Z;
        ( *m[1] ) = 2.0f * X * Y - 2.0f * Z * W;
        ( *m[2] ) = 2.0f * X * Z + 2.0f * Y * W;
        ( *m[3] ) = 0.0f;

        ( *m[4] ) = 2.0f * X * Y + 2.0f * Z * W;
        ( *m[5] ) = 1.0f - 2.0f * X * X - 2.0f * Z * Z;
        ( *m[6] ) = 2.0f * Y * Z - 2.0f * X * W;
        ( *m[7] ) = 0.0f;

        ( *m[8] ) = 2.0f * X * Z - 2.0f * Y * W;
        ( *m[9] ) = 2.0f * Y * Z + 2.0f * X * W;
        ( *m[10] ) = 1.0f - 2.0f * X * X - 2.0f * Y * Y;
        ( *m[11] ) = 0.0f;

        ( *m[12] ) = 0.0f;
        ( *m[13] ) = 0.0f;
        ( *m[14] ) = 0.0f;
        ( *m[15] ) = 1.0f;

        return m;
    }

    Vector4 Quaternion::ToVector4() const {
        return Vector4( X, Y, Z, W );
    }

    Matrix4x4 Quaternion::Rotation( const Vector3& center ) const {
        float sqw = W * W;
        float sqx = X * X;
        float sqy = Y * Y;
        float sqz = Z * Z;

        float m00 = sqx - sqy - sqz + sqw;
        float m11 = -sqx + sqy - sqz + sqw;
        float m22 = -sqx - sqy + sqz + sqw;

        float tmp1 = X * Y;
        float tmp2 = Z * W;
        float m01 = 2.0f * ( tmp1 + tmp2 );
        float m10 = 2.0f * ( tmp1 - tmp2 );

        tmp1 = X * Z;
        tmp2 = Y * W;
        float m02 = 2.0f * ( tmp1 - tmp2 );
        float m20 = 2.0f * ( tmp1 + tmp2 );

        tmp1 = Y * Z;
        tmp2 = X * W;
        float m12 = 2.0f * ( tmp1 + tmp2 );
        float m21 = 2.0f * ( tmp1 - tmp2 );

        float a1 = center.X;
        float a2 = center.Y;
        float a3 = center.Z;

        float m03 = a1 - a1 * m00 - a2 * m01 - a3 * m02;
        float m13 = a2 - a1 * m10 - a2 * m11 - a3 * m12;
        float m23 = a3 - a1 * m20 - a2 * m21 - a3 * m22;
        float m30, m31, m32;
        m30 = m31 = m32 = 0.0f;
        float m33 = 1.0f;

        // NOTE: Might have these backwards.
        Matrix4x4 m;
        ( *m[0] ) = m00;
        ( *m[1] ) = m01;
        ( *m[2] ) = m02;
        ( *m[3] ) = m03;

        ( *m[4] ) = m10;
        ( *m[5] ) = m11;
        ( *m[6] ) = m12;
        ( *m[7] ) = m13;

        ( *m[8] ) = m20;
        ( *m[9] ) = m21;
        ( *m[10] ) = m22;
        ( *m[11] ) = m23;

        ( *m[12] ) = m30;
        ( *m[13] ) = m31;
        ( *m[14] ) = m32;
        ( *m[15] ) = m33;
        return m;
    }

    Rotator Quaternion::ToRotator() const {
        Rotator euler;

        float qw = W;
        float qx = X;
        float qy = Y;
        float qz = Z;
        float sqw = qw * qw;
        float sqx = qx * qx;
        float sqy = qy * qy;
        float sqz = qz * qz;
        float unit = sqx + sqy + sqz + sqw; // If normalised is one, otherwise is correction factor
        float test = qx * qy + qz * qw;
        if( test > 86.3f * unit ) { // Singularity at north pole, 86.3 (1.56207) deg cutoff
            euler.Yaw = TMath::RadToDeg( 2.0f * atan2f( qx, qw ) );
            euler.Pitch = TMath::RadToDeg( 1.5708f );
            euler.Roll = TMath::RadToDeg( 0.0f );
            return euler;
        }
        if( test < -86.3f * unit ) { // Singularity at south pole, -86.3 (0.499) deg cutoff
            euler.Yaw = TMath::RadToDeg( -2.0f * atan2f( qx, qw ) );
            euler.Pitch = TMath::RadToDeg( -1.5708f );
            euler.Roll = TMath::RadToDeg( 0.0f );
            return euler;
        }
        euler.Yaw = TMath::RadToDeg( atan2f( 2.0f * qy * qw - 2.0f * qx * qz, sqx - sqy - sqz + sqw ) );
        euler.Pitch = TMath::RadToDeg( asinf( 2.0f * test / unit ) );
        euler.Roll = TMath::RadToDeg( atan2f( 2.0f * qx * qw - 2.0f * qy * qz, -sqx + sqy - sqz + sqw ) );
        return euler;
    }

    Quaternion Quaternion::Normalized( const Quaternion& q ) {
        Quaternion oq = q;
        oq.Normalize();
        return oq;
    }

    Quaternion Quaternion::FromRotator( const Rotator& rotator ) {
        return rotator.ToQuaternion();
    }

    Quaternion Quaternion::FromAxisAngle( const Vector3& axis, const float angle, bool normalize ) {
        Quaternion q;
        float factor = sinf( angle );
        q.X = 1 * factor;
        q.Y = 0 * factor;
        q.Z = 0 * factor;
        q.W = cosf( angle / 2.0f );
        if( normalize ) {
            q.Normalize();
        }

        return q;
    }

    Quaternion Quaternion::Slerp( const Quaternion& q0, const Quaternion& q1, float percentage ) {
        // Source: https://en.wikipedia.org/wiki/Slerp
        // Only unit quaternions are valid rotations.
        // Normalize to avoid undefined behavior.
        Quaternion v0 = q0;
        Quaternion v1 = q1;
        v0.Normalize();
        v1.Normalize();

        // Compute the cosine of the angle between the two vectors.
        float dot = v0.DotProduct( v1 );

        // If the dot product is negative, slerp won't take
        // the shorter path. Note that v1 and -v1 are equivalent when
        // the negation is applied to all four components. Fix by 
        // reversing one quaternion.
        if( dot < 0.0f ) {
            v1 = -v1;
            dot = -dot;
        }

        const float DOT_THRESHOLD = 0.9995f;
        if( dot > DOT_THRESHOLD ) {
            // If the inputs are too close for comfort, linearly interpolate
            // and normalize the result.

            Quaternion result = v0 + ( ( v1 - v0 ) * percentage );
            result.Normalize();
            return result;
        }

        // Since dot is in range [0, DOT_THRESHOLD], acos is safe
        float theta_0 = acosf( dot );        // theta_0 = angle between input vectors
        float theta = theta_0 * percentage;          // theta = angle between v0 and result
        float sin_theta = sinf( theta );     // compute this value only once
        float sin_theta_0 = sinf( theta_0 ); // compute this value only once

        float s0 = cosf( theta ) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
        float s1 = sin_theta / sin_theta_0;

        return ( v0 * s0 ) + ( v1 * s1 );
    }

    Quaternion Quaternion::FromString( const char* str ) {
        Quaternion q;
        std::vector<std::string> parts;
        StringUtilities::Split( str, ' ', &parts );
        U64 partsSize = parts.size();
        if( partsSize >= 1 ) {
            q.X = StringUtilities::ToFloat( parts[0] );
        }
        if( partsSize >= 2 ) {
            q.Y = StringUtilities::ToFloat( parts[1] );
        }
        if( partsSize >= 3 ) {
            q.Z = StringUtilities::ToFloat( parts[2] );
        }
        if( partsSize >= 4 ) {
            q.W = StringUtilities::ToFloat( parts[3] );
        }
        return q;
    }
}