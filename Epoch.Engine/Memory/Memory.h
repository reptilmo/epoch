#pragma once

#include "../Defines.h"
#include "../Types.h"

namespace Epoch {

    class TMemory {
    public:

        static FORCEINLINE void Free( void* block ) {
            free( block );
        }

        static FORCEINLINE void* Memcpy( void* destination, const void* source, U64 size ) {
            return memcpy( destination, source, size );
        }

        static FORCEINLINE void* MemSet( void* destination, U8 character, U64 size ) {
            return memset( destination, character, size );
        }

        static FORCEINLINE void* MemZero( void* destination, U64 size ) {
            return memset( destination, 0, size );
        }
    };


}