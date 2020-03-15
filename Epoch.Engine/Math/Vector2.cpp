
#include "../Logger.h"
#include "../Defines.h"
#include "../String/StringUtilities.h"
#include "TMath.h"

#include "Vector2.h"

namespace Epoch {

    Vector2::Vector2() {
        X = Y = 0;
    }

    Vector2::Vector2( const float xy ) {
        X = xy;
        Y = xy;
    }

    Vector2::Vector2( const float x, const float y ) {
        X = x;
        Y = y;
    }

    float Vector2::Length() const {
        return TMath::SquareRoot( LengthSquared() );
    }

    float Vector2::LengthSquared() const {
        return X * X + Y * Y;
    }

    Vector2 Vector2::Normalize() {
        const float length = Length();
        X /= length;
        Y /= length;
        return *this;
    }

    void Vector2::Set( const float x, const float y ) {
        X = x;
        Y = y;
    }

    const bool Vector2::Compare( const Vector2& v, const float tolerance ) const {
        if( TMath::Abs( X - v.X ) > tolerance ) {
            return false;
        }

        if( TMath::Abs( Y - v.Y ) > tolerance ) {
            return false;
        }

        return true;
    }

    Vector2 Vector2::operator=( const Vector2& v ) {
        X = v.X;
        Y = v.Y;
        return *this;
    }

    Vector2 Vector2::operator*( const float& scalar ) const {
        return Vector2( X * scalar, Y * scalar );
    }

    Vector2 Vector2::operator/( const float& scalar ) const {
        return Vector2( X / scalar, Y / scalar );
    }

    Vector2 Vector2::operator*( const Vector2& v ) const {
        return Vector2( X * v.X, Y * v.Y );
    }

    Vector2 Vector2::operator/( const Vector2& v ) const {
        return Vector2( X / v.X, Y / v.Y );
    }

    Vector2 Vector2::operator+( const Vector2& v ) const {
        return Vector2( X + v.X, Y + v.Y );
    }

    Vector2 Vector2::operator-( const Vector2& v ) const {
        return Vector2( X - v.X, Y - v.Y );
    }

    Vector2 Vector2::operator*=( const Vector2& v ) {
        X *= v.X;
        Y *= v.Y;
        return *this;
    }

    Vector2 Vector2::operator/=( const Vector2& v ) {
        X /= v.X;
        Y /= v.Y;
        return *this;
    }

    Vector2 Vector2::operator+=( const Vector2& v ) {
        X += v.X;
        Y += v.Y;
        return *this;
    }

    Vector2 Vector2::operator-=( const Vector2& v ) {
        X -= v.X;
        Y -= v.Y;
        return *this;
    }

    const bool Vector2::operator==( const Vector2& v ) const {
        return X == v.X && Y == v.Y;
    }

    const bool Vector2::operator!=( const Vector2& v ) const {
        return X != v.X && Y != v.Y;
    }

    float& Vector2::operator[]( I32 index ) {
        ASSERT_MSG( index < 2 && index >= 0, "Vector 2 index out of bounds" );
        return ( &X )[index];
    }

    float Vector2::operator[]( I32 index ) const {
        ASSERT_MSG( index < 2 && index >= 0, "Vector 2 index out of bounds" );
        return ( &X )[index];
    }

    Vector2 Vector2::Zero() {
        return Vector2();
    }

    Vector2 Vector2::One() {
        return Vector2( 1, 1 );
    }

    const float Vector2::Distance( Vector2& a, Vector2& b ) {
        Vector2 d = a - b;
        return d.Length();
    }

    Vector2 Vector2::Normalized( const Vector2& v ) {
        Vector2 ret = v;
        return ret.Normalize();
    }

    Vector2 Vector2::FromString( const char* str ) {
        Vector2 v;
        std::vector<std::string> parts;
        StringUtilities::Split( str, ' ', &parts );
        U64 partsSize = parts.size();
        if( partsSize >= 1 ) {
            v.X = StringUtilities::ToFloat( parts[0] );
        }
        if( partsSize >= 2 ) {
            v.Y = StringUtilities::ToFloat( parts[1] );
        }
        return v;
    }
}