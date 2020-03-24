#include "Types.h"
#include "Defines.h"
#include "Engine.h"

/**
 * The main entry point into the application.
 */
int main( int argc, const char** argv ) {
    Epoch::Engine engine( "Epoch" );
    engine.Run();
    return 0;
}