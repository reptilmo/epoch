#pragma once

#include "../Defines.h"
#include "../Types.h"

#ifndef TSTRING_DEFAULT_BUFFER_SIZE

// The default size of a string buffer.
#define TSTRING_DEFAULT_BUFFER_SIZE 20
#endif

namespace Epoch {

    enum class StringComparison {
        CaseSensitive,
        CaseInsensitive
    };

    /**
     * A custom string class which is used to represent strings throughout the system.
     */
    class EPOCH_API TString {
    public:
        TString();
        TString( const char* str );
        TString( const TString& other );

        explicit TString( const bool b );
        explicit TString( const char c );
        explicit TString( const U8 u );
        explicit TString( const U16 u );
        explicit TString( const U32 u );
        explicit TString( const U64 u );
        explicit TString( const I8 u );
        explicit TString( const I16 u );
        explicit TString( const I32 u );
        explicit TString( const I64 u );
        explicit TString( const F32 f );
        explicit TString( const F64 f );

        ~TString();

        const char* CStr() const;

        /**
         * Indicates if this string is equal to other using the provided comparison method. Default: StringComparison::CaseSensitive
         *
         * @param other The string to compare against.
         * @param comparison The comparison method to be used. Default: StringComparison::CaseSensitive
         *
         * @returns True if the strings match; otherwise false.
         */
        const bool Equals( const TString& other, const StringComparison comparison = StringComparison::CaseSensitive );

        const bool IsEmpty() const;

        void Append( const char c );
        void Append( const TString& str );
        void Append( const char* str, U64 length );

        void Clear();
        void Fill( const char fillChar, const U32 length );

        void Truncate( const U32 length );
        TString Left( const U32 length ) const;
        TString Right( const U32 length ) const;
        TString Mid( const U32 start, const U32 length ) const;

        TString& StripFilename();
        TString& StripPath();
        TString& StripFileExtension();
        TString ExtractFilename() const;
        TString ExtractFilePath() const;
        TString ExtractFileExtension() const;

        static TString Format( const char* format, ... );
        static I32 vsnPrintf( char* dest, I32 size, const char* fmt, va_list argptr );
        friend int tvsprintf( TString& dest, const char* fmt, va_list ap );

    public:
        const U32 Length() const;
        const U32 Length();

        const char* Data() const;
        char* Data();


    public:
        static const I32 Compare( const TString& string1, const TString& string2 );
        static const I32 Compare( const char* string1, const TString& string2 );
        static const I32 Compare( const TString& string1, const char* string2 );
        static const I32 Compare( const char* string1, const char* string2 );
        static const I32 CompareN( const TString& string1, const TString& string2, const U32 number );
        static const I32 CompareN( const char* string1, const TString& string2, const U32 number );
        static const I32 CompareN( const TString& string1, const char* string2, const U32 number );
        static const I32 CompareN( const char* string1, const char* string2, const U32 number );

        static const I32 ICompare( const TString& string1, const TString& string2 );
        static const I32 ICompare( const char* string1, const TString& string2 );
        static const I32 ICompare( const TString& string1, const char* string2 );
        static const I32 ICompare( const char* string1, const char* string2 );
        static const I32 ICompareN( const TString& string1, const TString& string2, const U32 number );
        static const I32 ICompareN( const char* string1, const TString& string2, const U32 number );
        static const I32 ICompareN( const TString& string1, const char* string2, const U32 number );
        static const I32 ICompareN( const char* string1, const char* string2, const U32 number );

        static void Append( char* destination, U64 size, const char* source );

        /**
         * A safe string copy which ensures a trailing zero.
         *
         * @param dest The destination character array.
         * @param src The source character array.
         * @param destinationSize The size of the destination buffer.
         */
        static void CopyEnsureTrailingZero( char* destination, const char* source, I64 destinationSize );

    public:
        operator const char* ( );
        operator const char* ( ) const;
        char operator[]( int index );
        const char& operator[]( int index ) const;

        void operator=( const char* str );
        void operator=( const TString& other );

        friend TString operator+( const TString& a, const TString& b );
        friend TString operator+( const TString& a, const char* b );
        friend TString operator+( const char* a, const TString& b );

        friend TString operator+( const TString& a, const F32 b );
        friend TString operator+( const TString& a, const F64 b );

        friend TString operator+( const TString& a, const U8 b );
        friend TString operator+( const TString& a, const U16 b );
        friend TString operator+( const TString& a, const U32 b );
        friend TString operator+( const TString& a, const U64 b );

        friend TString operator+( const TString& a, const I8 b );
        friend TString operator+( const TString& a, const I16 b );
        friend TString operator+( const TString& a, const I32 b );
        friend TString operator+( const TString& a, const I64 b );

        friend TString operator+( const TString& a, const bool b );

        friend const bool operator==( const TString& a, const TString& b );
        friend const bool operator==( const TString& a, const char* b );
        friend const bool operator==( const char* a, const TString& b );

        friend const bool operator!=( const TString& a, const TString& b );
        friend const bool operator!=( const TString& a, const char* b );
        friend const bool operator!=( const char* a, const TString& b );

        TString& operator+=( const F32 f );
        TString& operator+=( const F64 f );

        TString& operator+=( const U8 u );
        TString& operator+=( const U16 u );
        TString& operator+=( const U32 u );
        TString& operator+=( const U64 u );

        TString& operator+=( const I8 i );
        TString& operator+=( const I16 i );
        TString& operator+=( const I32 i );
        TString& operator+=( const I64 i );

        TString& operator+=( const bool b );


    private:
        void buildDefault();
        void ensureAllocated( const U32 size, const bool keepData = true );
        void reallocate( const U32 size, const bool keepData );
        void freeData();
    private:
        U32 _allocated = 0;
        U32 _length = 0;
        char* _data = nullptr;
        char defaultBuffer[TSTRING_DEFAULT_BUFFER_SIZE];
    };

    FORCEINLINE const char* TString::CStr() const {
        return _data;
    }

    FORCEINLINE const U32 TString::Length() const {
        return _length;
    }

    FORCEINLINE const U32 TString::Length() {
        return _length;
    }

    FORCEINLINE const char* TString::Data() const {
        return _data;
    }

    FORCEINLINE char* TString::Data() {
        return _data;
    }

    FORCEINLINE TString::operator const char* ( ) {
        return CStr();
    }

    FORCEINLINE TString::operator const char* ( ) const {
        return CStr();
    }

    FORCEINLINE char TString::operator[]( int index ) {
        ASSERT( ( index >= 0 ) && ( index <= (I32)_length ) );
        return _data[index];
    }

    FORCEINLINE const char& TString::operator[]( int index ) const {
        ASSERT( ( index >= 0 ) && ( index <= (I32)_length ) );
        return _data[index];
    }
}