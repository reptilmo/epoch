#include "StringUtilities.h"
#include "../Types.h"

#include <stdarg.h>

namespace Epoch {

    const char* StringUtilities::Format( const char* str, ... ) {
        va_list args;

        int size = 100;
        int n;
        char* p, * np;
        if( ( p = (char*)malloc( size ) ) == nullptr ) {
            return nullptr;
        }

        while( true ) {
            // Attempt to print in allocated space.
            va_start( args, str );
            n = vsnprintf( p, size, str, args );
            va_end( args );

            // If it worked, return the string
            if( n > -1 && n < size ) {
                return p;
            }

            // If not, allocate more space and try again
            if( n > -1 ) {
                // Size is precisely what is needed, but not enough for \0
                size = n + 1;
            } else {
                // Double the size
                size *= 2;
            }

            // Realloc and try again
            if( ( np = (char*)realloc( p, size ) ) == NULL ) {

                // Failed to realloc, return
                free( p );
                return nullptr;
            } else {
                p = np;
            }
        }
    }

    void StringUtilities::Split( const std::string& str, const char delimiter, std::vector<std::string>* parts ) {
        U64 start;
        U64 end = 0;

        while( ( start = str.find_first_not_of( delimiter, end ) ) != std::string::npos ) {
            end = str.find( delimiter, start );
            parts->push_back( str.substr( start, end - start ) );
        }
    }

    F32 StringUtilities::ToFloat( const std::string& str ) {
        return (F32)atof( str.c_str() );
    }
}