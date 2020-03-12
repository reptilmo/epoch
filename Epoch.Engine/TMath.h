#pragma once

#include "Types.h"

namespace Epoch {

    /**
     * Hold various math-related utility functions.
     */
    class TMath {
    public:

        /**
         * Clamps value between min and max, inclusive.
         * 
         * @param value The value to be clamped.
         * @param min The minimum value.
         * @param max The maximum value.
         * 
         * @returns The clamped value.
         */
        static U32 ClampU32( const U32 value, const U32 min, const U32 max );
    };

    inline U32 TMath::ClampU32( const U32 value, const U32 min, const U32 max ) {
        if( value < min ) {
            return min;
        }
        if( value > max ) {
            return max;
        }
        return value;
    }
}