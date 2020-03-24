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
    };


}