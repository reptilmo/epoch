#include "pch.h"
#include "CppUnitTest.h"

#include <Containers/LinkedList.h>
#include <Types.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Epoch;

namespace EpochEngineTest
{

    TEST_CLASS( LinkedListTest ) {
public:

    TEST_METHOD( AllOperations ) {
        LinkedList<int> list;

        list.InsertAt( 9, 0 );
        list.Push( 1 );
        list.Push( 2 );
        list.Append( 3 );
        list.InsertAt( 4, 1 );
        list.RemoveAt( 2 );
        list.RemoveByValue( 2 );
        
        
    }

    };
}
