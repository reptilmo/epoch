#pragma once

#include "../Types.h"

namespace Epoch {

    struct Rectangle2D {
    public:
        I32 Left;
        I32 Right;
        I32 Top;
        I32 Bottom;

        Rectangle2D() : Left( 0 ), Right( 0 ), Top( 0 ), Bottom( 0 ) {
        }

        Rectangle2D( const I32 left, const I32 top, const I32 right, const I32 bottom ) {
            Set( left, top, right, bottom );
        }

        void Set( const I32 left, const I32 top, const I32 right, const I32 bottom ) {
            Left = left;
            Top = top;
            Right = right;
            Bottom = bottom;
        }
    };
}