#include "TMath.h"

#include <cstdint>

namespace Epoch {

    // Constant definitions

    // PI
    const F32 TMath::PI = 3.14159265358979323846f;

    // PI * 2
    const F32 TMath::PI_2 = 2.0f * TMath::PI;

    // PI / 2
    const F32 TMath::HALF_PI = 0.5f * TMath::PI;

    // PI / 4
    const F32 TMath::QUARTER_PI = 0.25f * TMath::PI;

    // 1 / PI
    const F32 TMath::ONE_OVER_PI = 1.0f / TMath::PI;

    // 1 / PI * 2
    const F32 TMath::ONE_OVER_TWO_PI = 1.0f / TMath::PI_2;

    // e
    const F32 TMath::E = 2.71828182845904523536f;

    // The square root of 2
    const F32 TMath::SQRT_TWO = 1.41421356237309504880f;;

    // The square root of 3
    const F32 TMath::SQRT_THREE = 1.73205080756887729352f;

    // The square root of 1 / 2
    const F32 TMath::SQRT_ONE_OVER_TWO = 0.70710678118654752440f;

    // The square root of 1 / 3
    const F32 TMath::SQRT_ONE_OVER_THREE = 0.57735026918962576450f;

    // The multiplier to convert degrees to radians.
    const F32 TMath::DEG2RAD_MULTIPLIER = TMath::PI / 180.0f;

    // The multiplier to convert radians to degrees.
    const F32 TMath::RAD2DEG_MULTIPLIER = 180.0f / TMath::PI;

    // The multiplier to convert seconds to milliseconds.
    const F32 TMath::SEC_TO_MS_MULTIPLIER = 1000.0f;

    // The multiplier to convert milliseconds to seconds.
    const F32 TMath::MS_TO_SEC_MULTIPLIER = 0.001f;

    // A huge number that should be larger than any valid number used.
    const F32 TMath::INFINITY = 1e30f;

    // Smallest positive number where 1.0 + FLOAT_EPSILON != 0
    const F32 TMath::FLOAT_EPSILON = 1.192092896e-07f;

    // Smallest normal 32-bit floating value.
    const F32 TMath::FLOAT_SMALLEST_NORMAL = *reinterpret_cast<const F32*>( &SMALLEST_NON_DENORMAL ); // 1.1754944e-038f
}