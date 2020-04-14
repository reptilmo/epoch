
#include <stdarg.h>
#include <stdio.h>

#include "../Defines.h"
#include "../Logger.h"

#ifndef TSTRING_SIZE_ALLOCATION_GRANULARITY
#define TSTRING_SIZE_ALLOCATION_GRANULARITY 32
#endif // !TSTRING_SIZE_ALLOCATION_GRANULARITY

#include "TString.h"

namespace Epoch {



    TString::TString() {
        buildDefault();
    }

    TString::TString( const char* str ) {
        buildDefault();

        if( str ) {
            U32 l = (U32)strlen( str );
            ensureAllocated( l + 1 );
            strcpy( _data, str );
            _length = l;
        }
    }

    TString::TString( const TString& other ) {
        buildDefault();
        U32 l = other.Length();
        ensureAllocated( l + 1 );
        strcpy( _data, other._data );
        _length = l;
    }

    TString::TString( const bool b ) {
        buildDefault();
        ensureAllocated( 2 );
        _data[0] = b ? '1' : '0';
        _data[1] = '\0';
        _length = 1;
    }

    TString::TString( const char c ) {
        buildDefault();
        ensureAllocated( 2 );
        _data[0] = c;
        _data[1] = '\0';
        _length = 1;
    }

    TString::TString( const U8 u ) {
        buildDefault();
        char text[64];
        U32 length = sprintf( text, "%u", u );
        ensureAllocated( length + 1 );
        strcpy( _data, text );
        _length = length;
    }
    TString::TString( const U16 u ) {
        buildDefault();
        char text[64];
        U32 length = sprintf( text, "%u", u );
        ensureAllocated( length + 1 );
        strcpy( _data, text );
        _length = length;
    }
    TString::TString( const U32 u ) {
        buildDefault();
        char text[64];
        U32 length = sprintf( text, "%u", u );
        ensureAllocated( length + 1 );
        strcpy( _data, text );
        _length = length;
    }
    TString::TString( const U64 u ) {
        buildDefault();
        char text[64];
        U32 length = sprintf( text, "%I64u", u );
        ensureAllocated( length + 1 );
        strcpy( _data, text );
        _length = length;
    }
    TString::TString( const I8 u ) {
        buildDefault();
        char text[64];
        U32 length = sprintf( text, "%d", u );
        ensureAllocated( length + 1 );
        strcpy( _data, text );
        _length = length;
    }
    TString::TString( const I16 u ) {
        buildDefault();
        char text[64];
        U32 length = sprintf( text, "%d", u );
        ensureAllocated( length + 1 );
        strcpy( _data, text );
        _length = length;
    }
    TString::TString( const I32 u ) {
        buildDefault();
        char text[64];
        U32 length = sprintf( text, "%d", u );
        ensureAllocated( length + 1 );
        strcpy( _data, text );
        _length = length;
    }
    TString::TString( const I64 u ) {
        buildDefault();
        char text[64];
        U32 length = sprintf( text, "%I64i", u );
        ensureAllocated( length + 1 );
        strcpy( _data, text );
        _length = length;
    }
    TString::TString( const F32 f ) {
        buildDefault();
        char text[64];
        U32 length = snprintf( text, sizeof( text ), "%f", f );

        // Strip trailing zeroes
        while( length > 0 && text[length - 1] == '0' ) {
            text[--length] = '\0';
        }

        // Strip decimal if need be.
        while( length > 0 && text[length - 1] == '.' ) {
            text[--length] = '\0';
        }

        ensureAllocated( length + 1 );
        strcpy( _data, text );
        _length = length;
    }

    TString::TString( const F64 f ) {
        buildDefault();
        char text[64];
        U32 length = snprintf( text, sizeof( text ), "%f", f );

        // Strip trailing zeroes
        while( length > 0 && text[length - 1] == '0' ) {
            text[--length] = '\0';
        }

        // Strip decimal if need be.
        while( length > 0 && text[length - 1] == '.' ) {
            text[--length] = '\0';
        }

        ensureAllocated( length + 1 );
        strcpy( _data, text );
        _length = length;
    }

    TString::~TString() {
        freeData();
    }

    const bool TString::Equals( const TString& other, const StringComparison comparison ) {
        switch( comparison ) {
        default:
        case StringComparison::CaseSensitive:
            return Compare( *this, other ) == 0;
        case StringComparison::CaseInsensitive:
            return ICompare( *this, other ) == 0;
        }
    }

    const bool TString::IsEmpty() const {
        return _length == 0;
    }

    void TString::Append( const char c ) {
        ensureAllocated( _length + 2 );
        _data[_length] = c;
        _length++;
        _data[_length] = '\0';
    }

    void TString::Append( const TString& str ) {
        U32 newLength = _length + str.Length();
        ensureAllocated( newLength + 1 );
        for( U32 i = 0; i < str.Length(); ++i ) {
            _data[_length + i] = str[i];
        }
        _length = newLength;
        _data[_length] = '\0';
    }

    void TString::Append( const char* str, U64 length ) {
        U32 newLength = 0;

        if( str && length ) {
            newLength = _length + 1;
            ensureAllocated( newLength + 1 );
            for( U32 i = 0; str[i] && i < length; ++i ) {
                _data[_length + i] = str[i];
            }
            _length = newLength;
            _data[_length] = '\0';
        }
    }

    void TString::Clear() {
        freeData();
        buildDefault();
    }

    TString TString::Format( const char* format, ... ) {
        TString result;
        va_list args;
        va_start( args, format );
        tvsprintf( result, format, args );
        va_end( args );
        return result;
    }

    I32 TString::vsnPrintf( char* dest, I32 size, const char* fmt, va_list argptr ) {
        int ret;

#ifdef _WIN32
        ret = _vsnprintf( dest, size - 1, fmt, argptr );
#else
        ret = vsnprintf( dest, size, fmt, argptr );
#endif
        dest[size - 1] = '\0';
        if( ret < 0 || ret >= size ) {
            return -1;
        }
        return ret;
    }

    int tvsprintf( TString& dest, const char* fmt, va_list ap ) {
        I32 length = 0;
        char buffer[32000];
        length = TString::vsnPrintf( buffer, sizeof( buffer ) - 1, fmt, ap );
        buffer[sizeof( buffer ) - 1] = '\0';

        dest = buffer;
        return length;
    }

    void TString::buildDefault() {
        _allocated = TSTRING_DEFAULT_BUFFER_SIZE;
        _data = defaultBuffer;
        _length = 0;
        _data[0] = '\0';
    }

    void TString::ensureAllocated( const U32 size, const bool keepData ) {
        if( size > _allocated ) {
            reallocate( size, keepData );
        }
    }

    void TString::reallocate( const U32 size, const bool keepData ) {
        char* newBuffer;
        U32 newSize;

        ASSERT( size > 0 );

        U32 mod = size % TSTRING_SIZE_ALLOCATION_GRANULARITY;
        if( !mod ) {
            newSize = size;
        } else {
            newSize = size + TSTRING_SIZE_ALLOCATION_GRANULARITY - mod;
        }

        newBuffer = new char[newSize];
        if( keepData && _data ) {
            _data[_length] = '\0';
            strcpy( newBuffer, _data );
        }

        if( _data && _data != defaultBuffer ) {
            delete[] _data;
        }

        _allocated = newSize;
        _data = newBuffer;
    }

    void TString::freeData() {
        if( _data && _data != defaultBuffer ) {
            delete[] _data;
            _data = defaultBuffer;
        }
    }

    void TString::operator=( const char* str ) {
        if( !str ) {
            ensureAllocated( 1, false );
            _data[0] = '\0';
            _length = 0;
            return;
        }

        // No need to copy if they are the same.
        if( str == _data ) {
            return;
        }

        // If aliasing, copy the data over and boot out.
        if( str >= _data && str <= _data + _length ) {
            U64 addrDiff = str - _data;

            ASSERT( strlen( str ) < (unsigned)_length );

            // Iterate until empty
            U64 i;
            for( i = 0; str[i]; ++i ) {
                _data[i] = str[i];
            }
            _data[i] = '\0';
            _length -= (U32)addrDiff;

            return;
        }

        U32 l = (U32)strlen( str );
        ensureAllocated( l + 1, false );
        strcpy( _data, str );
        _length = l;
    }

    void TString::operator=( const TString& other ) {
        U32 length = other.Length();
        ensureAllocated( length + 1, false );
        memcpy( _data, other.Data(), (U64)length );
        _data[length] = '\0';
        _length = length;
    }

    TString operator+( const TString& a, const TString& b ) {
        TString result( a );
        result.Append( b );
        return result;
    }
    TString operator+( const TString& a, const char* b ) {
        TString result( a );
        result.Append( b );
        return result;
    }
    TString operator+( const char* a, const TString& b ) {
        TString result( a );
        result.Append( b );
        return result;
    }

    TString operator+( const TString& a, const F32 b ) {
        char text[64];
        TString result( a );

        sprintf( text, "%f", b );
        result.Append( text );
        return result;
    }

    TString operator+( const TString& a, const F64 b ) {
        char text[64];
        TString result( a );

        sprintf( text, "%f", b );
        result.Append( text );
        return result;
    }

    TString operator+( const TString& a, const U8 b ) {
        char text[64];
        TString result( a );

        sprintf( text, "%u", b );
        result.Append( text );
        return result;
    }
    TString operator+( const TString& a, const U16 b ) {
        char text[64];
        TString result( a );

        sprintf( text, "%u", b );
        result.Append( text );
        return result;
    }
    TString operator+( const TString& a, const U32 b ) {
        char text[64];
        TString result( a );

        sprintf( text, "%u", b );
        result.Append( text );
        return result;
    }
    TString operator+( const TString& a, const U64 b ) {
        char text[64];
        TString result( a );

        sprintf( text, "%I64u", b );
        result.Append( text );
        return result;
    }

    TString operator+( const TString& a, const I8 b ) {
        char text[64];
        TString result( a );

        sprintf( text, "%i", b );
        result.Append( text );
        return result;
    }
    TString operator+( const TString& a, const I16 b ) {
        char text[64];
        TString result( a );

        sprintf( text, "%i", b );
        result.Append( text );
        return result;
    }
    TString operator+( const TString& a, const I32 b ) {
        char text[64];
        TString result( a );

        sprintf( text, "%i", b );
        result.Append( text );
        return result;
    }
    TString operator+( const TString& a, const I64 b ) {
        char text[64];
        TString result( a );

        sprintf( text, "%I64i", b );
        result.Append( text );
        return result;
    }

    TString operator+( const TString& a, const bool b ) {
        TString result( a );
        result.Append( b ? "true" : "false" );
        return result;
    }

    const bool operator==( const TString& a, const TString& b ) {
        return TString::Compare( a, b ) == 0;
    }
    const bool operator==( const TString& a, const char* b ) {
        return TString::Compare( a, b ) == 0;
    }
    const bool operator==( const char* a, const TString& b ) {
        return TString::Compare( a, b ) == 0;
    }

    const bool operator!=( const TString& a, const TString& b ) {
        return !( a == b );
    }
    const bool operator!=( const TString& a, const char* b ) {
        return !( a == b );
    }
    const bool operator!=( const char* a, const TString& b ) {
        return !( a == b );
    }

    TString& TString::operator+=( const F32 f ) {
        char text[64];
        sprintf( text, "%f", f );
        Append( text );
        return *this;
    }
    TString& TString::operator+=( const F64 f ) {
        char text[64];
        sprintf( text, "%f", f );
        Append( text );
        return *this;
    }

    TString& TString::operator+=( const U8 u ) {
        char text[64];
        sprintf( text, "%u", u );
        Append( text );
        return *this;
    }
    TString& TString::operator+=( const U16 u ) {
        char text[64];
        sprintf( text, "%u", u );
        Append( text );
        return *this;
    }
    TString& TString::operator+=( const U32 u ) {
        char text[64];
        sprintf( text, "%u", u );
        Append( text );
        return *this;
    }
    TString& TString::operator+=( const U64 u ) {
        char text[64];
        sprintf( text, "%I64u", u );
        Append( text );
        return *this;
    }

    TString& TString::operator+=( const I8 i ) {
        char text[64];
        sprintf( text, "%i", i );
        Append( text );
        return *this;
    }
    TString& TString::operator+=( const I16 i ) {
        char text[64];
        sprintf( text, "%i", i );
        Append( text );
        return *this;
    }
    TString& TString::operator+=( const I32 i ) {
        char text[64];
        sprintf( text, "%i", i );
        Append( text );
        return *this;
    }
    TString& TString::operator+=( const I64 i ) {
        char text[64];
        sprintf( text, "%I64i", i );
        Append( text );
        return *this;
    }

    TString& TString::operator+=( const bool b ) {
        Append( b ? "true" : "false" );
        return *this;
    }


    /********************************************* STATIC METHODS ***************************************/
    const I32 TString::Compare( const TString& string1, const TString& string2 ) {
        return Compare( string1.Data(), string2.Data() );
    }

    const I32 TString::Compare( const char* string1, const TString& string2 ) {
        return Compare( string1, string2.Data() );
    }

    const I32 TString::Compare( const TString& string1, const char* string2 ) {
        return Compare( string1.Data(), string2 );
    }

    const I32 TString::Compare( const char* string1, const char* string2 ) {
        return (I32)strcmp( string1, string2 );
    }

    const I32 TString::CompareN( const TString& string1, const TString& string2, const U32 number ) {
        return CompareN( string1.Data(), string2.Data(), number );
    }

    const I32 TString::CompareN( const char* string1, const TString& string2, const U32 number ) {
        return CompareN( string1, string2.Data(), number );
    }

    const I32 TString::CompareN( const TString& string1, const char* string2, const U32 number ) {
        return CompareN( string1.Data(), string2, number );
    }

    const I32 TString::CompareN( const char* string1, const char* string2, const U32 number ) {
        return (I32)strncmp( string1, string2, number );
    }

    const I32 TString::ICompare( const TString& string1, const TString& string2 ) {
        return ICompare( string1.Data(), string2.Data() );
    }

    const I32 TString::ICompare( const char* string1, const TString& string2 ) {
        return ICompare( string1, string2.Data() );
    }

    const I32 TString::ICompare( const TString& string1, const char* string2 ) {
        return ICompare( string1.Data(), string2 );
    }

    const I32 TString::ICompare( const char* string1, const char* string2 ) {
        return (I32)_stricmp( string1, string2 );
    }

    const I32 TString::ICompareN( const TString& string1, const TString& string2, const U32 number ) {
        return ICompareN( string1.Data(), string2.Data(), number );
    }

    const I32 TString::ICompareN( const char* string1, const TString& string2, const U32 number ) {
        return ICompareN( string1, string2.Data(), number );
    }

    const I32 TString::ICompareN( const TString& string1, const char* string2, const U32 number ) {
        return ICompareN( string1.Data(), string2, number );
    }

    const I32 TString::ICompareN( const char* string1, const char* string2, const U32 number ) {
        return (I32)_strnicmp( string1, string2, number );
    }

    void TString::Append( char* destination, U64 size, const char* source ) {
        U64 length = strlen( destination );
        if( length >= size ) {
            Logger::Error( "TString::Append - unable to append because destination buffer overflows." );
            return;
        }

        CopyEnsureTrailingZero( destination + length, source, size - length );
    }

    void TString::CopyEnsureTrailingZero( char* destination, const char* source, I64 destinationSize ) {
        if( !source ) {
            Logger::Warn( "TString::CopyEnsureTrailingZero attempting to copy from null source." );
            return;
        }

        if( destinationSize < 1 ) {
            Logger::Warn( "TString::CopyEnsureTrailingZero attempting to copy to a destination with size < 1." );
            return;
        }

        strncpy( destination, source, destinationSize );
        destination[destinationSize - 1] = 0;
    }
}

#ifdef TSTRING_SIZE_ALLOCATION_GRANULARITY
#undef TSTRING_SIZE_ALLOCATION_GRANULARITY
#endif // !TSTRING_SIZE_ALLOCATION_GRANULARITY

#ifdef TSTRING_DEFAULT_BUFFER_SIZE
#undef TSTRING_DEFAULT_BUFFER_SIZE
#endif