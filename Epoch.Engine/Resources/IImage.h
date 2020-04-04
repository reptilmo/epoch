#pragma once

#include "../Types.h"

namespace Epoch {

    class IImage {
    public:
        virtual const U32 GetWidth() const = 0;
        virtual const U32 GetHeight() const = 0;
    };
}