#include "Types.h"
#include "Defines.h"
#include "Engine.h"

/**
 * The main entry point into the application.
 */
int main( int argc, const char** argv ) {
    Epoch::Engine* engine = new Epoch::Engine( "Epoch" );
    engine->Run();
    delete engine;
    return 0;
}