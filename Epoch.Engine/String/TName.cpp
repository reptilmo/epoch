
#include <map>

#include "../Types.h"

#include "TString.h"

#include "TName.h"

namespace Epoch {

    static bool namePoolInitialized = false;
    static TNamePool* NamePoolData;
    static TNamePool& GetNamePool() {
        if( !namePoolInitialized ) {
            NamePoolData = new TNamePool();
            namePoolInitialized = true;
        }
        return *NamePoolData;
    }

    std::map<U64, TName> _namesTable;

    TName::TName( const TString& str ) {

    }

    /*const TName& TName::Find( const TString& str ) {
        TNamePool& pool = GetNamePool();
    }*/


    TNamePool::TNamePool() {

    }

    void TNamePool::InitializeTemp() {
        auto pool = GetNamePool();
    }
}