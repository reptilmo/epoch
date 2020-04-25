#include "../Defines.h"
#include "../Logger.h"
#include "../String/StringUtilities.h"

#include "Vector3.h"
#include "TMath.h"

namespace Epoch {

    Vector3::Vector3() {
        X = Y = Z = 0;
    }

    Vector3::Vector3( const F32 xyz ) {
        X = xyz;
        Y = xyz;
        Z = xyz;
    }

    Vector3::Vector3( const Vector3& other ) {
        X = other.X;
        Y = other.Y;
        Z = other.Z;
    }

    Vector3::Vector3( const F32 x, const F32 y, const F32 z ) {
        X = x;
        Y = y;
        Z = z;
    }

    F32 Vector3::Length() const {
        return TMath::SquareRoot( LengthSquared() );
    }

    F32 Vector3::LengthSquared() const {
        return ( X * X + Y * Y + Z * Z );
    }

    Vector3 Vector3::Normalize() {
        const F32 length = Length();
        if( length != 0.0f ) {
            X /= length;
            Y /= length;
            Z /= length;
        }
        return *this;
    }

    Vector3 Vector3::Cross( const Vector3& other ) {
        X = Y * other.Z - Z * other.Y;
        Y = Z * other.X - X * other.Z;
        Z = X * other.Y - Y * other.X;
        return *this;
    }

    F32 Vector3::Dot( const Vector3& other ) const {
        F32 p = 0;
        p += X * other.X;
        p += Y * other.Y;
        p += Z * other.Z;
        return p;
    }

    void Vector3::Set( const F32 x, const F32 y, const F32 z ) {
        X = x;
        Y = y;
        Z = z;
    }

    const char* Vector3::ToString() const {
        return StringUtilities::Format( "%f %f %f", X, Y, Z );
    }

    const bool Vector3::Compare( const Vector3& v, const F32 tolerance ) const {
        if( TMath::Abs( X - v.X ) > tolerance ) {
            return false;
        }

        if( TMath::Abs( Y - v.Y ) > tolerance ) {
            return false;
        }

        if( TMath::Abs( Z - v.Z ) > tolerance ) {
            return false;
        }

        return true;
    }

    Vector3 Vector3::operator=( const Vector3& v ) {
        X = v.X;
        Y = v.Y;
        Z = v.Z;
        return *this;
    }

    Vector3 Vector3::operator*( const F32& scalar ) const {
        return Vector3( X * scalar, Y * scalar, Z * scalar );
    }

    Vector3 Vector3::operator/( const F32& scalar ) const {
        return Vector3( X / scalar, Y / scalar, Z / scalar );
    }

    Vector3 Vector3::operator*( const Vector3& v ) const {
        return Vector3( X * v.X, Y * v.Y, Z * v.Z );
    }

    Vector3 Vector3::operator/( const Vector3& v ) const {
        return Vector3( X / v.X, Y / v.Y, Z / v.Z );
    }

    Vector3 Vector3::operator+( const Vector3& v ) const {
        return Vector3( X + v.X, Y + v.Y, Z + v.Z );
    }

    Vector3 Vector3::operator-( const Vector3& v ) const {
        return Vector3( X - v.X, Y - v.Y, Z - v.Z );
    }

    Vector3 Vector3::operator*=( const Vector3& v ) {
        X *= v.X;
        Y *= v.Y;
        Z *= v.Z;
        return *this;
    }

    Vector3 Vector3::operator/=( const Vector3& v ) {
        X /= v.X;
        Y /= v.Y;
        Z /= v.Z;
        return *this;
    }

    Vector3 Vector3::operator+=( const Vector3& v ) {
        X += v.X;
        Y += v.Y;
        Z += v.Z;
        return *this;
    }

    Vector3 Vector3::operator-=( const Vector3& v ) {
        X -= v.X;
        Y -= v.Y;
        Z -= v.Z;
        return *this;
    }

    const bool Vector3::operator==( const Vector3& v ) const {
        return X == v.X && Y == v.Y && Z == v.Z;
    }

    const bool Vector3::operator!=( const Vector3& v ) const {
        return X != v.X && Y != v.Y && Z != v.Z;
    }

    F32& Vector3::operator[]( I32 index ) {
        ASSERT_MSG( index < 3 && index >= 0, "Vector 3 index out of bounds" );
        return ( &X )[index];
    }

    F32 Vector3::operator[]( I32 index ) const {
        ASSERT_MSG( index < 3 && index >= 0, "Vector 3 index out of bounds" );
        return ( &X )[index];
    }

    /*
     Static methods.
    */


    Vector3 Vector3::Zero() {
        return Vector3();
    }

    Vector3 Vector3::One() {
        return Vector3( 1, 1, 1 );
    }

    Vector3 Vector3::Up() {
        return Vector3( 0, 1, 0 );
    }

    Vector3 Vector3::Down() {
        return Vector3( 0, -1, 0 );
    }

    Vector3 Vector3::Left() {
        return Vector3( -1, 0, 0 );
    }

    Vector3 Vector3::Right() {
        return Vector3( 1, 0, 0 );
    }

    Vector3 Vector3::Forward() {
        return Vector3( 0, 0, 1 );
    }

    Vector3 Vector3::Backward() {
        return Vector3( 0, 0, -1 );
    }

    F32 Vector3::Distance( const Vector3& a, const Vector3& b ) {
        Vector3 d = a - b;
        return d.Length();
    }

    Vector3 Vector3::Normalized( const Vector3& v ) {
        Vector3 ret = v;
        return ret.Normalize();
    }

    Vector3 Vector3::Cross( const Vector3& a, const Vector3& b ) {
        Vector3 v;
        v.X = a.Y * b.Z - a.Z * b.Y;
        v.Y = a.Z * b.X - a.X * b.Z;
        v.Z = a.X * b.Y - a.Y * b.X;
        return v;
    }

    Vector3 Vector3::FromString( const char* str ) {
        Vector3 v;
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
        return v;
    }
}