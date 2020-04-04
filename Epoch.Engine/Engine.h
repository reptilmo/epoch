#pragma once

#include "Types.h"

namespace Epoch {

    class Platform;

    /**
     * The main class which manages everything in the system. One of these is created
     * by the calling application.
     */
    class Engine final {
    public:

        /**
         * Creates a new Engine. Run() must be called after this to kick things off.
         * 
         * @param applicationName The name of the application. This is used in the created window.
         */
        Engine( const char* applicationName );

        /**
         * The default destructor.
         */
        ~Engine();

        /**
         * Returns a pointer to the platform layer owned by this engine.
         */
        Platform* GetPlatform() { return _platform; }

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

    private:
        Platform* _platform;
    };
}