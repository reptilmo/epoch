#pragma once

#include "Defines.h"
#include "Types.h"

namespace Epoch {

    class IApplication;
    class World;

    /**
     * The main class which manages everything in the system. One of these is created
     * by the calling application.
     */
    class EPOCH_API Engine final {
    public:

        /**
         * Creates a new Engine. Run() must be called after this to kick things off.
         * 
         * @param application A pointer to the instance of the application.
         */
        Engine( IApplication* application );

        /**
         * The default destructor.
         */
        ~Engine();

        /**
         * Returns a pointer to the application layer owned by this engine.
         */
        IApplication* GetApplication() { return _application; }

        /**
         * Kicks off engine initialization and ultimately the game loop.
         */
        void Run();

        /**
         * A callback made from the platform layer during the event poll loop.
         * This is the main game loop.
         *
         * @param deltaTime The time in seconds since the last loop.
         */
        const bool OnLoop( const F32 deltaTime );

        void Shutdown();

    private:
        IApplication* _application;
        World* _world;
    };
}