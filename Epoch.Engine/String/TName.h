#pragma once

#include "../Defines.h"

namespace Epoch {

    class TString;

    class EPOCH_API TName {
    public:
        TName( const TString& str );

    public:
        //static const TName& Find( const TString& str );
    };

    class EPOCH_API TNamePool {
    public:
        static void InitializeTemp();
        TNamePool();
    private:
    };
}