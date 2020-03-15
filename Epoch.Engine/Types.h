#pragma once

typedef unsigned char       byte;  // 8 bits
typedef unsigned short      word;  // 16 bits
typedef unsigned int        dword; // 32 bits

// Unsigned 64-bit integer
typedef unsigned long long  U64;

// Unsigned 32-bit integer
typedef unsigned int        U32;

// Unsigned 16-bit integer
typedef unsigned short      U16;

// Unsigned 8-bit integer
typedef unsigned char       U8;


// Signed 64-bit integer
typedef signed long long    I64;

// Signed 32-bit integer
typedef signed int          I32;

// Signed 16-bit integer
typedef signed short        I16;

// Signed 8-bit integer
typedef signed char         I8;

// 64-bit floating point
typedef double              F64;

// 32-bit floating point
typedef float               F32;

namespace Epoch {

    /**
     * Used to represent the extents (or size) of something using signed integers.
     */
    struct Extent2D {
        I32 Width;
        I32 Height;
    };
}