#include "pch.h"
#include "CppUnitTest.h"

#include <Containers/List.h>
#include <Types.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Epoch;

namespace EpochEngineTest
{

    TEST_CLASS( ListTest ) {
public:

    TEST_METHOD( AllOperations ) {
        List<int> list;
        list.Add( 0 );
        list.Add( 1 );
        list.Add( 2 ); // should be 0, 1, 2

        list.InsertAt( 3, 1 ); // Should be 0, 3, 1, 2
        I32 removed = list.Remove( 1 );// Should be 0, 3, 2, return 2
        list.RemoveAt( 0 ); // Should be 3,2
        list.RemoveAt( 1 ); // Should be 3
        
        list.Add( 4 );
        list.Add( 5 );
        list.Add( 6 );

        list.Add( 5 );
        list.Add( 7 ); // Should be 3,4,5,6,5,7

        I32 index = list.IndexOf( 5 ); // Should be 2
        index = list.LastIndexOf( 5 ); // Should be 4
        I32 size = list.Size();// should be 6

        list.Clear();
        list.Add( 1 );
        list.Shrink();


    }

    };
}