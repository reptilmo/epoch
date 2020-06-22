#pragma once

// Require SSE2
#include <emmintrin.h>

/*
 * Float 4 vector register type where the first float (X) is stored in the lowest 32 bits
 */
typedef __m128 VectorRegister;