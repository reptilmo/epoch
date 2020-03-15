
#include <string>
#include <vector>

#include "../Logger.h"
#include "../String/StringUtilities.h"

#include "Vector4.h"

namespace Epoch {

    Vector4::Vector4() {
        X = Y = Z = W = 0;
    }

    Vector4::Vector4( const float xyzw ) {
        X = xyzw;
        Y = xyzw;
        Z = xyzw;
        W = xyzw;
    }

    Vector4::Vector4( const float x, const float y, const float z, const float w ) {
        X = x;
        Y = y;
        Z = z;
        W = w;
    }

    void Vector4::Set( const float x, const float y, const float z, const float w ) {
        X = x;
        Y = y;
        Z = z;
        W = w;
    }

    Vector4 Vector4::operator=( const Vector4& v ) {
        X = v.X;
        Y = v.Y;
        Z = v.Z;
        W = v.W;
        return *this;
    }

    Vector4 Vector4::operator*( const float& scalar ) const {
        return Vector4( X * scalar, Y * scalar, Z * scalar, W * scalar );
    }

    Vector4 Vector4::operator/( const float& scalar ) const {
        return Vector4( X / scalar, Y / scalar, Z / scalar, W / scalar );
    }

    Vector4 Vector4::operator*( const Vector4& v ) const {
        return Vector4( X * v.X, Y * v.Y, Z * v.Z, W * v.W );
    }

    Vector4 Vector4::operator/( const Vector4& v ) const {
        return Vector4( X / v.X, Y / v.Y, Z / v.Z, W / v.W );
    }

    Vector4 Vector4::operator+( const Vector4& v ) const {
        return Vector4( X + v.X, Y + v.Y, Z + v.Z, W + v.W );
    }

    Vector4 Vector4::operator-( const Vector4& v ) const {
        return Vector4( X - v.X, Y - v.Y, Z - v.Z, W - v.W );
    }

    Vector4 Vector4::operator*=( const Vector4& v ) {
        X *= v.X;
        Y *= v.Y;
        Z *= v.Z;
        W *= v.W;
        return *this;
    }

    Vector4 Vector4::operator/=( const Vector4& v ) {
        X /= v.X;
        Y /= v.Y;
        Z /= v.Z;
        W /= v.W;
        return *this;
    }

    Vector4 Vector4::operator+=( const Vector4& v ) {
        X += v.X;
        Y += v.Y;
        Z += v.Z;
        W += v.W;
        return *this;
    }

    Vector4 Vector4::operator-=( const Vector4& v ) {
        X -= v.X;
        Y -= v.Y;
        Z -= v.Z;
        W -= v.W;
        return *this;
    }

    const bool Vector4::operator==( const Vector4& v ) const {
        return X == v.X && Y == v.Y && Z == v.Z && W == v.W;
    }

    const bool Vector4::operator!=( const Vector4& v ) const {
        return X != v.X && Y != v.Y && Z != v.Z && W != v.W;
    }

    float& Vector4::operator[]( I32 index ) {
        ASSERT_MSG( index < 4 && index >= 0, "Vector 4 index out of bounds" );
        return ( &X )[index];
    }

    float Vector4::operator[]( I32 index ) const {
        ASSERT_MSG( index < 4 && index >= 0, "Vector 4 index out of bounds" );
        return ( &X )[index];
    }

    Vector4 Vector4::Zero() {
        return Vector4();
    }

    Vector4 Vector4::One() {
        return Vector4( 1, 1, 1, 1 );
    }

    Vector4 Vector4::FromString( const char* str ) {
        Vector4 v;
        std::vector<std::string> parts;
        StringUtilities::Split( str, ' ', &parts );
        U64 partsSize = parts.size();
        if( partsSize >= 1 ) {
            v.X = StringUtilities::ToFloat( parts[0] );
        }
        if( partsSize >= 2 ) {
            v.Y = StringUtilities::ToFloat( parts[1] );
        }
        if( partsSize >= 3 ) {
            v.Z = StringUtilities::ToFloat( parts[2] );
        }
        if( partsSize >= 4 ) {
            v.W = StringUtilities::ToFloat( parts[3] );
        }
        return v;
    }
}