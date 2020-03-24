#pragma once

#include "../Types.h"
#include "../Containers/LinkedList.h"

namespace Epoch {

    template<class T>
    struct DynamicBlock {
        U64 HeapIndex;
        U64 BlockSize;
        U64 Offset;
        T Data;
    };

    template <class T>
    class DynamicBlockAllocator {
    public:
        const bool GetBlockAt( const U64 heapIndex, DynamicBlock* block );
        void Initialize( const U64 size );
        const bool Allocate( const U64 offset, const U64 size, DynamicBlock* block );
        const bool Free( const U64 offset, const U64 free );
        const bool FreeBlock( const DynamicBlock* block );
        const bool FreeIndex( const U64 heapIndex );
    private:
        U64 _heapIndex = 0;
        U64 _totalSize = size;
    };
}