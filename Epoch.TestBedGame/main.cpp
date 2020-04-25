#include <Types.h>
#include <Defines.h>
#include <Engine.h>
#include <Platform/IApplication.h>

/**
 * The main entry point into the application.
 */
int main( int argc, const char** argv ) {

    // Request an application to be created.
    Epoch::IApplication* application = Epoch::Application::CreateApplication( L"Epoch Engine" );

    // Kick things off by running the application.
    application->Run();
    return 0;
}