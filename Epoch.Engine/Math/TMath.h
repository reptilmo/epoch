#pragma once

#include <math.h>


#include "../Types.h"
#include "../Defines.h"

#define INT8_SIGN_BIT 7
#define INT16_SIGN_BIT 15
#define I32_SIGN_BIT 31
#define I64_SIGN_BIT 63

#define INT8_SIGN_MASK ( 1 << INT8_SIGN_BIT )
#define INT16_SIGN_MASK ( 1 << INT16_SIGN_BIT )
#define I32_SIGN_MASK ( 1UL << I32_SIGN_BIT )
#define I64_SIGN_MASK ( 1ULL << I64_SIGN_BIT )

#ifdef INFINITY
#undef INFINITY
#endif

template<class T> T Max( T x, T y ) { return ( x > y ) ? x : y; }
template<class T> T Min( T x, T y ) { return ( x < y ) ? x : y; }

namespace Epoch {

    /**
     * Hold various math-related utility functions.
     */
    class TMath {
    public:

        // Inverse square root (32-bit)
        static F32 InverseSquareRoot( const F32 x );

        // Inverse square root (64-bit)
        static F64 InverseSquareRoot( const F64 x );

        // Square root (32-bit)
        static F32 SquareRoot( const F32 x );

        // Square root (64-bit)
        static F64 SquareRoot( const F64 x );

        // Sine (32-bit)
        static F32 Sin( const F32 x );

        // Sine (64-bit)
        static F64 Sin( const F64 x );

        // Cosine (32-bit)
        static F32 Cos( const F32 x );

        // Cosine (64-bit)
        static F64 Cos( const F64 x );

        // Sine and cosine (32-bit)
        static void SinCos( const F32 n, F32& s, F32& c );

        // Sine and cosine (64-bit)
        static void SinCos( const F64 n, F64& s, F64& c );

        // Tangent (32-bit)
        static F32 Tan( const F32 x );

        // Tangent (64-bit)
        static F64 Tan( const F64 x );

        // Arc sine (32-bit)
        static F32 ASin( const F32 x );

        // Arc sine (64-bit)
        static F64 ASin( const F64 x );

        static F32 ATan2( const F32 x, const F32 y );
        static F64 ATan2( const F64 x, const F64 y );

        // Arc cosine (32-bit) - input clamped to [-1, 1] to avoid silent NaN
        static F32 ACos( const F32 x );

        // Arc cosine (64-bit)
        static F64 ACos( const F64 x );

        // Arc tangent (32-bit)
        static F32 ATan( const F32 x );

        // Arc tangent (64-bit)
        static F64 ATan( const F64 x );

        // Arc tangent (32-bit)
        static F32 ATan( const F32 x, const F32 y );

        // Arc tangent (64-bit)
        static F64 ATan( const F64 x, const F64 y );

        // x raised to the power of y (32-bit)
        static F32 Pow( const F32 x, const F32 y );

        // x raised to the power of y (64-bit)
        static F64 Pow( const F64 x, const F64 y );

        // e raised to the power of f (32-bit)
        static F32 Exp( const F32 f );

        // e raised to the power of f (64-bit)
        static F64 Exp( const F64 f );

        // Natural logarithm (32-bit)
        static F32 Log( const F32 x );

        // Natural logarithm (64-bit)
        static F64 Log( const F64 x );

        // Integral x raised to the power of y
        static int IPow( int x, int y );

        // Integral base-2 logarithm of the provided floating point value.
        static int ILog2( F32 x );

        // Integral base-2 logarithm of the provided integer value.
        static int ILog2( I32 x );

        // Round x down to the nearest power of 2.
        static int FloorPowerOfTwo( int x );

        // Round x up to the nearest power of 2.
        static int CeilPowerOfTwo( int x );

        // Indicates if x is a power of 2.
        static bool IsPowerOfTwo( int x );

        // Returns the absolute value for the provided integer x.
        static int Abs( const int x );

        // Returns the absolute value for the provided floating-point x.
        static F32 Abs( const F32 x );

        // Returns the absolute value for the provided F64 floating-point x.
        static F64 Abs( const F64 x );

        // Returns the largest integer that is less than or equal to the provided value.
        static F32 Floor( const F32 x );

        // F64 the largest integer that is less than or equal to the provided value.
        static F64 Floor( const F64 x );

        // Returns the smallest integer that is greater than or equal to the provided value.
        static F32 Ceil( const F32 x );

        // Returns the smallest integer that is greater than or equal to the provided value.
        static F64 Ceil( const F64 x );

        // Returns the nearest integer.
        static F32 NearestInt( const F32 x );

        // Returns the nearest integer.
        static F64 NearestInt( const F64 x );

        // f - Floor( f )
        static F32 Frac( F32 f );

        // Converts the provided float to a 64-bit integer
        static I64 Ftoi64( F32 f );

        // Converts the provided float to a 32-bit integer
        static I32 Ftoi32( F32 f );

        // Converts the provided float to a 16-bit integer
        static I16 Ftoi16( F32 f );

        // Converts the provided float to a 8-bit integer
        static I8 Ftoi8( F32 f );

        // Converts the provided float to an unsigned 64-bit integer
        static U64 Ftoui64( F32 f );

        // Converts the provided float to an unsigned 32-bit integer
        static U32 FtoU32( F32 f );

        // Converts the provided float to an unsigned 16-bit integer
        static U16 Ftoui16( F32 f );

        // Converts the provided float to an unsigned 8-bit integer
        static U8 Ftoui8( F32 f );

        // Converts the provided float to a byte (clamped to [0-255]
        static byte Ftob( F32 f );

        // Clamps provided value i to a 8-bit range [-127, 128]
        static I8 ClampI8( int i );

        // Clamps provided value i to a 16-bit range [-127, 128]
        static I16 ClampI16( int i );

        // Clamps provided value i to a 32-bit range [-127, 128]
        static I32 ClampI32( int i );

        // Clamps value inclusively between min and max (8-bit).
        static I8 ClampI8( const I8 value, const I8 min, const I8 max );

        // Clamps value inclusively between min and max (16-bit).
        static I16 ClampI16( const I16 value, const I16 min, const I16 max );

        // Clamps value inclusively between min and max (32-bit).
        static I32 ClampI32( const I32 value, const I32 min, const I32 max );

        // Clamps value inclusively between min and max (64-bit).
        static I64 ClampI64( const I64 value, const I64 min, const I64 max );

        // Clamps value inclusively between min and max (8-bit).
        static U8 ClampU8( const U8 value, const U8 min, const U8 max );

        // Clamps value inclusively between min and max (16-bit).
        static U16 ClampU16( const U16 value, const U16 min, const U16 max );

        // Clamps value inclusively between min and max (32-bit).
        static U32 ClampU32( const U32 value, const U32 min, const U32 max );

        // Clamps value inclusively between min and max (64-bit).
        static U64 ClampU64( const U64 value, const U64 min, const U64 max );

        // Clamps value inclusively between min and max (32-bit).
        static F32 ClampFloat32( const F32 value, const F32 min, const F32 max );

        // Clamps value inclusively between min and max (64-bit).
        static F64 ClampFloat64( const F64 value, const F64 min, const F64 max );

        static F32 AngleNormalize360( F32 angle );
        static F32 AngleNormalize180( F32 angle );

        static F32 AngleDelta( const F32 angle1, const F32 angle2 );

        // Converts degrees to radians.
        static F32 DegToRad( const F32 degrees );

        // Converts radians to degrees.
        static F32 RadToDeg( const F32 radians );

    public:
        // Constants

        // PI
        static const F32 PI;

        // PI * 2
        static const F32 PI_2;

        // PI / 2
        static const F32 HALF_PI;

        // PI / 4
        static const F32 QUARTER_PI;

        // 1 / PI
        static const F32 ONE_OVER_PI;

        // 1 / PI * 2
        static const F32 ONE_OVER_TWO_PI;

        // e
        static const F32 E;

        // The square root of 2
        static const F32 SQRT_TWO;

        // The square root of 3
        static const F32 SQRT_THREE;

        // The square root of 1 / 2
        static const F32 SQRT_ONE_OVER_TWO;

        // The square root of 1 / 3
        static const F32 SQRT_ONE_OVER_THREE;

        // The multiplier to convert degrees to radians.
        static const F32 DEG2RAD_MULTIPLIER;

        // The multiplier to convert radians to degrees.
        static const F32 RAD2DEG_MULTIPLIER;

        // The multiplier to convert seconds to milliseconds.
        static const F32 SEC_TO_MS_MULTIPLIER;

        // The multiplier to convert milliseconds to seconds.
        static const F32 MS_TO_SEC_MULTIPLIER;

        // A huge number that should be larger than any valid number used.
        static const F32 INFINITY;

        // Smallest positive number where 1.0 + FLOAT_EPSILON != 0
        static const F32 FLOAT_EPSILON;

        // Smallest normal 32-bit floating value.
        static const F32 FLOAT_SMALLEST_NORMAL;
    };

    // Inverse square root (32-bit)
    inline F32 TMath::InverseSquareRoot( const F32 x ) {
        return ( x > FLOAT_SMALLEST_NORMAL ) ? sqrtf( 1.0f / x ) : TMath::INFINITY;
    }

    // Inverse square root (64-bit)
    inline F64 TMath::InverseSquareRoot( const F64 x ) {
        return ( x > FLOAT_SMALLEST_NORMAL ) ? sqrt( 1.0 / x ) : TMath::INFINITY;
    }

    // Square root (32-bit)
    inline F32 TMath::SquareRoot( const F32 x ) {
        return ( x >= 0.0f ) ? sqrtf( x ) : 0.0f;
    }

    // Square root (64-bit)
    inline F64 TMath::SquareRoot( const F64 x ) {
        return ( x >= 0.0 ) ? sqrt( x ) : 0.0;
    }

    // Sine (32-bit)
    inline F32 TMath::Sin( const F32 x ) {
        return sinf( x );
    }

    // Sine (64-bit)
    inline F64 TMath::Sin( const F64 x ) {
        return sin( x );
    }

    // Cosine (32-bit)
    inline F32 TMath::Cos( const F32 x ) {
        return cosf( x );
    }

    // Cosine (64-bit)
    inline F64 TMath::Cos( const F64 x ) {
        return cos( x );
    }

    // Sine and cosine (32-bit)
    inline void TMath::SinCos( const F32 n, F32& s, F32& c ) {
        s = sinf( n );
        c = cosf( n );
    }

    // Sine and cosine (64-bit)
    inline void TMath::SinCos( const F64 n, F64& s, F64& c ) {
        s = sin( n );
        c = cos( n );
    }

    // Tangent (32-bit)
    inline F32 TMath::Tan( const F32 x ) {
        return tanf( x );
    }

    // Tangent (64-bit)
    inline F64 TMath::Tan( const F64 x ) {
        return tan( x );
    }

    // Arc sine (32-bit)
    inline F32 TMath::ASin( const F32 x ) {
        if( x <= -1.0f ) {
            return -HALF_PI;
        }
        if( x >= 1.0f ) {
            return HALF_PI;
        }
        return asinf( x );
    }

    // Arc sine (64-bit)
    inline F64 TMath::ASin( const F64 x ) {
        if( x <= -1.0 ) {
            return -(F64)HALF_PI;
        }
        if( x >= 1.0 ) {
            return (F64)HALF_PI;
        }
        return asin( x );
    }

    inline F32 TMath::ATan2( const F32 x, const F32 y ) {
        // return atan2f( x, y );
        // atan2f occasionally returns NaN with perfectly valid input (possibly due to a compiler or library bug).
        // We are replacing it with a minimax approximation with a max relative error of 7.15255737e-007 compared to the C library function.
        // On PC this has been measured to be 2x faster than the std C version.

        const float absX = TMath::Abs( x );
        const float absY = TMath::Abs( y );
        const bool yAbsBigger = ( absY > absX );
        float t0 = yAbsBigger ? absY : absX; // Max(absY, absX)
        float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)

        if( t0 == 0.0f ) {
            return 0.0f;
        }

        float t3 = t1 / t0;
        float t4 = t3 * t3;

        static const float c[7] = {
            +7.2128853633444123e-03f,
            -3.5059680836411644e-02f,
            +8.1675882859940430e-02f,
            -1.3374657325451267e-01f,
            +1.9856563505717162e-01f,
            -3.3324998579202170e-01f,
            +1.0f
        };

        t0 = c[0];
        t0 = t0 * t4 + c[1];
        t0 = t0 * t4 + c[2];
        t0 = t0 * t4 + c[3];
        t0 = t0 * t4 + c[4];
        t0 = t0 * t4 + c[5];
        t0 = t0 * t4 + c[6];
        t3 = t0 * t3;

        t3 = yAbsBigger ? ( 0.5f * TMath::PI ) - t3 : t3;
        t3 = ( x < 0.0f ) ? PI - t3 : t3;
        t3 = ( y < 0.0f ) ? -t3 : t3;

        return t3;
    }

    inline F64 TMath::ATan2( const F64 x, const F64 y ) {
        return atan2( x, y );
    }

    // Arc cosine (32-bit) - input clamped to [-1, 1] to avoid silent NaN
    inline F32 TMath::ACos( const F32 x ) {
        if( x <= -1.0f ) {
            return -PI;
        }
        if( x >= 1.0f ) {
            return 0.0f;
        }
        return acosf( x );
    }

    // Arc cosine (64-bit)
    inline F64 TMath::ACos( const F64 x ) {
        if( x <= -1.0 ) {
            return -PI;
        }
        if( x >= 1.0 ) {
            return 0.0;
        }
        return acos( x );
    }

    // Arc tangent (32-bit)
    inline F32 TMath::ATan( const F32 x ) {
        return atanf( x );
    }

    // Arc tangent (64-bit)
    inline F64 TMath::ATan( const F64 x ) {
        return atan( x );
    }

    // Arc tangent (32-bit)
    inline F32 TMath::ATan( const F32 x, const F32 y ) {
        ASSERT_DEBUG( fabs( y ) > TMath::FLOAT_SMALLEST_NORMAL || fabs( x ) > TMath::FLOAT_SMALLEST_NORMAL );
        return atan2f( y, x );
    }

    // Arc tangent (64-bit)
    inline F64 TMath::ATan( const F64 x, const F64 y ) {
        ASSERT_DEBUG( abs( y ) > TMath::FLOAT_SMALLEST_NORMAL || abs( x ) > TMath::FLOAT_SMALLEST_NORMAL );
        return atan2( y, x );
    }

    // x raised to the power of y (32-bit)
    inline F32 TMath::Pow( const F32 x, const F32 y ) {
        return powf( x, y );
    }

    // x raised to the power of y (64-bit)
    inline F64 TMath::Pow( const F64 x, const F64 y ) {
        return pow( x, y );
    }

    // e raised to the power of f (32-bit)
    inline F32 TMath::Exp( const F32 f ) {
        return expf( f );
    }

    // e raised to the power of f (64-bit)
    inline F64 TMath::Exp( const F64 f ) {
        return exp( f );
    }

    // Natural logarithm (32-bit)
    inline F32 TMath::Log( const F32 x ) {
        return logf( x );
    }

    // Natural logarithm (64-bit)
    inline F64 TMath::Log( const F64 x ) {
        return log( x );
    }

    // Round x down to the nearest power of 2.
    inline int TMath::FloorPowerOfTwo( int x ) {
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x++;
        return x >> 1;
    }

    // Round x up to the nearest power of 2.
    inline int TMath::CeilPowerOfTwo( int x ) {
        x--;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x++;
        return x;
    }

    // Indicates if x is a power of 2.
    inline bool TMath::IsPowerOfTwo( int x ) {
        return ( x & ( x - 1 ) ) == 0 && x > 0;
    }

    // Returns the absolute value for the provided integer x.
    inline int TMath::Abs( const int x ) {
        return abs( x );
    }

    // Returns the absolute value for the provided floating-point x.
    inline F32 TMath::Abs( const F32 x ) {
        return fabsf( x );
    }

    // Returns the absolute value for the provided F64 floating-point x.
    inline F64 TMath::Abs( const F64 x ) {
        return abs( x );
    }

    // Returns the largest integer that is less than or equal to the provided value.
    inline F32 TMath::Floor( const F32 x ) {
        return floorf( x );
    }

    // F64 the largest integer that is less than or equal to the provided value.
    inline F64 TMath::Floor( const F64 x ) {
        return floor( x );
    }

    // Returns the smallest integer that is greater than or equal to the provided value.
    inline F32 TMath::Ceil( const F32 x ) {
        return ceilf( x );
    }

    // Returns the smallest integer that is greater than or equal to the provided value.
    inline F64 TMath::Ceil( const F64 x ) {
        return ceil( x );
    }

    // Returns the nearest integer.
    inline F32 TMath::NearestInt( const F32 x ) {
        return floorf( x + 0.5f );
    }

    // Returns the nearest integer.
    inline F64 TMath::NearestInt( const F64 x ) {
        return floor( x + 0.5f );
    }

    // f - Floor( f )
    inline F32 TMath::Frac( F32 f ) {
        return f - floorf( f );
    }

    // Converts the provided float to a 64-bit integer
    inline I64 TMath::Ftoi64( F32 f ) {
        return (I64)f;
    }

    // Converts the provided float to a 32-bit integer
    inline I32 TMath::Ftoi32( F32 f ) {
        I64 i = (I64)f;
        if( i < -2147483648 ) {
            return -2147483648;
        } else if( i > 2147483647 ) {
            return 2147483647;
        }
        return (I32)f;
    }

    // Converts the provided float to a 16-bit integer
    inline I16 TMath::Ftoi16( F32 f ) {
        I64 i = (I64)f;
        if( i < -32768 ) {
            return -32768;
        } else if( i > 32767 ) {
            return 32767;
        }
        return (I16)f;
    }

    // Converts the provided float to a 8-bit integer
    inline I8 TMath::Ftoi8( F32 f ) {
        I64 i = (I64)f;
        if( i < -128 ) {
            return -128;
        } else if( i > 127 ) {
            return 127;
        }
        return (I8)f;
    }

    // Converts the provided float to an unsigned 64-bit integer
    inline U64 TMath::Ftoui64( F32 f ) {
        return (U64)f;
    }

    // Converts the provided float to an unsigned 32-bit integer
    inline U32 TMath::FtoU32( F32 f ) {
        I64 i = (I64)f;
        if( i < 0 ) {
            return 0;
        } else if( i > 4294967295 ) {
            return 4294967295;
        }
        return (I32)f;
    }

    // Converts the provided float to an unsigned 16-bit integer
    inline U16 TMath::Ftoui16( F32 f ) {
        I64 i = (I64)f;
        if( i < 0 ) {
            return 0;
        } else if( i > 65535 ) {
            return 65535;
        }
        return (I16)f;
    }

    // Converts the provided float to an unsigned 8-bit integer
    inline U8 TMath::Ftoui8( F32 f ) {
        I64 i = (I64)f;
        if( i < 0 ) {
            return 0;
        } else if( i > 255 ) {
            return 255;
        }
        return (I8)f;
    }

    // Converts the provided float to a byte (clamped to [0-255]
    inline byte TMath::Ftob( F32 f ) {
        I64 i = (I64)f;
        if( i < 0 ) {
            return 0;
        } else if( i > 255 ) {
            return 255;
        }
        return (byte)f;
    }

    // Clamps provided value i to a 8-bit range [-127, 128]
    inline I8 TMath::ClampI8( int i ) {
        if( i < -128 ) {
            return -128;
        }
        if( i > 127 ) {
            return 127;
        }
        return (I8)i;
    }

    // Clamps provided value i to a 16-bit range [-127, 128]
    inline I16 TMath::ClampI16( int i ) {
        if( i < -32768 ) {
            return -32768;
        }
        if( i > 32767 ) {
            return 32767;
        }
        return (I16)i;
    }

    // Clamps provided value i to a 32-bit range [-127, 128]
    inline I32 TMath::ClampI32( int i ) {
        if( i < -2147483648 ) {
            return -2147483648;
        } else if( i > 2147483647 ) {
            return 2147483647;
        }
        return (I32)i;
    }

    // Clamps value inclusively between min and max (8-bit).
    inline I8 TMath::ClampI8( const I8 value, const I8 min, const I8 max ) {
        if( value < min ) {
            return min;
        }
        if( value > max ) {
            return max;
        }
        return value;
    }

    // Clamps value inclusively between min and max (16-bit).
    inline I16 TMath::ClampI16( const I16 value, const I16 min, const I16 max ) {
        if( value < min ) {
            return min;
        }
        if( value > max ) {
            return max;
        }
        return value;
    }

    // Clamps value inclusively between min and max (32-bit).
    inline I32 TMath::ClampI32( const I32 value, const I32 min, const I32 max ) {
        if( value < min ) {
            return min;
        }
        if( value > max ) {
            return max;
        }
        return value;
    }

    // Clamps value inclusively between min and max (64-bit).
    inline I64 TMath::ClampI64( const I64 value, const I64 min, const I64 max ) {
        if( value < min ) {
            return min;
        }
        if( value > max ) {
            return max;
        }
        return value;
    }

    // Clamps value inclusively between min and max (8-bit).
    inline U8 TMath::ClampU8( const U8 value, const U8 min, const U8 max ) {
        if( value < min ) {
            return min;
        }
        if( value > max ) {
            return max;
        }
        return value;
    }

    // Clamps value inclusively between min and max (16-bit).
    inline U16 TMath::ClampU16( const U16 value, const U16 min, const U16 max ) {
        if( value < min ) {
            return min;
        }
        if( value > max ) {
            return max;
        }
        return value;
    }

    // Clamps value inclusively between min and max (32-bit).
    inline U32 TMath::ClampU32( const U32 value, const U32 min, const U32 max ) {
        if( value < min ) {
            return min;
        }
        if( value > max ) {
            return max;
        }
        return value;
    }

    // Clamps value inclusively between min and max (64-bit).
    inline U64 TMath::ClampU64( const U64 value, const U64 min, const U64 max ) {
        if( value < min ) {
            return min;
        }
        if( value > max ) {
            return max;
        }
        return value;
    }

    // Clamps value inclusively between min and max (32-bit).
    inline F32 TMath::ClampFloat32( const F32 value, const F32 min, const F32 max ) {
        if( value < min ) {
            return min;
        }
        if( value > max ) {
            return max;
        }
        return value;
    }

    // Clamps value inclusively between min and max (64-bit).
    inline F64 TMath::ClampFloat64( const F64 value, const F64 min, const F64 max ) {
        return Max<F64>( min, Min<F64>( max, value ) );
    }

    inline F32 TMath::AngleNormalize360( F32 angle ) {
        if( ( angle >= 360.0f ) || ( angle < 0.0f ) ) {
            angle -= floorf( angle * ( 1.0f / 360.0f ) ) * 360.0f;
        }
        return angle;
    }
    inline F32 TMath::AngleNormalize180( F32 angle ) {
        angle = AngleNormalize360( angle );
        if( angle > 180.0f ) {
            angle -= 360.0f;
        }
        return angle;
    }

    inline F32 TMath::AngleDelta( const F32 angle1, const F32 angle2 ) {
        return AngleNormalize180( angle1 - angle2 );
    }

    // Converts degrees to radians.
    inline F32 TMath::DegToRad( const F32 degrees ) {
        return degrees * DEG2RAD_MULTIPLIER;
    }

    // Converts radians to degrees.
    inline F32 TMath::RadToDeg( const F32 radians ) {
        return radians * RAD2DEG_MULTIPLIER;
    }
}