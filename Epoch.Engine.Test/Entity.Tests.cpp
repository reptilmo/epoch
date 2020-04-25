#include "pch.h"
#include "CppUnitTest.h"

#include <Types.h>
#include <Game.h>
#include <String/TString.h>
#include <World/Entity.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Epoch;

namespace EpochEngineTest
{

    TEST_CLASS( EntityTest ) {
public:

    TEST_METHOD( AllOperations ) {
        TString name( "test" );
        Entity* entity = Entity::Create( name );
        entity->Name.Equals( "test" );
        Entity::Free( entity );

    }

    };
}