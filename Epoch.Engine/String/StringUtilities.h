#pragma once

#include <string>
#include <vector>

#include "../Types.h"
#include "TString.h"

namespace Epoch {

    /**
     * A collection of string-related utilities.
     */
    class StringUtilities {
    public:

        /**
         * Formats the given string using the provided input variables.
         *
         * @param str The string to format.
         * @param ... The input parameters.
         *
         * @returns The formatted string.
         */
        static const char* Format( const char* str, ... );

        /**
         * Splits the provided string using the given delimiter and places the results into parts.
         *
         * @param str The string to be split.
         * @param delimiter The delimiter to split by.
         * @param parts A pointer to the result of this split operation. Pass an empty vector here.
         */
        static void Split( const std::string& str, const char delimiter, std::vector<std::string>* parts );

        /**
         * Converts the provided string to a 32-bit floating-point number.
         *
         * @returns A 32-bit floating-point number
         */
        static F32 ToFloat( const std::string& str );
    };

    /**
     * Used for string comparisons in std containers such as map.
     */
    struct TStringCompare {
    public:
        bool operator() ( const TString& str1, const TString& str2 ) const {
            return TString::Compare( str1, str2 ) < 0;
        }
    };
}