#include <Types.h>
#include <Defines.h>
#include <Engine.h>
#include <Platform/IApplication.h>

// TEST
#include <String/TName.h>

/**
 * The main entry point into the application.
 */
int main( int argc, const char** argv ) {
    Epoch::TNamePool::InitializeTemp();
    /*Epoch::IApplication* application = Epoch::Application::CreateApplication( Epoch Engine );
    application->Run();*/
    Epoch::Engine engine( "Epoch" );
    engine.Run();
    return 0;
}