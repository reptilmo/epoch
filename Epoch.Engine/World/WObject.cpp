
#include "../Memory/Memory.h"

#include "WObject.h"


namespace Epoch {

    U32 WObject::GLOBAL_OBJECT_ID = 0;

    WObject* WObject::Allocate( U64 size, U64 alignment ) {
        WObject* result = static_cast<WObject*>( TMemory::AllocateAligned( size, alignment ) );

        // Obtain a global unique ID.
        result->_id = WObject::GLOBAL_OBJECT_ID++;
        return result;
    }

    void WObject::Free( WObject* object ) {
        // Manually call destructor.
        object->~WObject();
        TMemory::FreeAligned( object );
        object = nullptr;
    }

    WObject::WObject() {

        // Obtain a global unique ID.
        _id = WObject::GLOBAL_OBJECT_ID++;
    }

    WObject::~WObject() {

        // Intentionally wrap around to U32_MAX
        _id = -1;
    }
}