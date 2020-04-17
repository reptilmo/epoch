

#include "World.h"

namespace Epoch {

    U32 WObject::GLOBAL_OBJECT_ID = 0;

    WObject::WObject() {

        // Obtain a global unique ID.
        _id = WObject::GLOBAL_OBJECT_ID++;
    }

    WObject::~WObject() {

        // Intentionally wrap around to U32_MAX
        _id = -1;
    }
}