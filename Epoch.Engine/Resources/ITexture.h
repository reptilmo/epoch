#pragma once

namespace Epoch {

    class ITexture {
    public:
        virtual const char* GetName() const = 0;
    };
}