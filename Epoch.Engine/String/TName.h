#pragma once


namespace Epoch {

    class TString;

    class TName {
    public:
        TName( const TString& str );

    public:
        //static const TName& Find( const TString& str );
    };

    class TNamePool {
    public:
        static void InitializeTemp();
        TNamePool();
    private:
    };
}