#pragma once

#include "../Defines.h"
#include "../Types.h"

namespace Epoch {

    /**
     * Represents the base for all objects which exist in the world. Every object is assigned a unique
     * numeric identifier which can later be used for retrieval of an instance by that identifier.
     */
    class EPOCH_API WObject {
    public:
        static WObject* Allocate( U64 size, U64 alignment = 16 );
        static void Free( WObject* object );
    public:
        const U32 GetId() const { return _id; }
    protected:
        WObject();
        virtual ~WObject();
    private:
        static U32 GLOBAL_OBJECT_ID;
    private:
        U32 _id;
    };
}