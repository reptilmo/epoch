#pragma once

#include "../../Types.h"

namespace Epoch {

    class Engine;
    class IRendererBackend;

    /*
     The renderer "front-end", which represents the interaction point with the rest of the engine.
     This is responsible for tasks such as scene object sorting, object culling (frustum, occlusion, etc)
     as well as building interaction tables to determine how lights interact with surfaces. This information
     is passed to this object's counterpart, the "back-end", which makes the required calls to the underlying
     grapics API.
    */
    class RendererFrontEnd final {
    public:

        /**
         * Creates a new renderer front end. 
         * 
         * @param engine A pointer to the engine which owns this renderer.
         */
        RendererFrontEnd( Engine* engine );

        /**
         * Default destructor.
         */
        ~RendererFrontEnd();

        /**
         * Initializes this renderer.
         *
         * @returns True if successful; otherwise false.
         */
        const bool Initialize();

        /**
         * Processes a single frame.
         *
         * @param deltaTime The amount of time in seconds since the last frame.
         *
         * @returns True on success, false on failure. Returning false crashes the application.
         */
        const bool Frame( const F32 deltaTime );

    private:

        // A pointer to the engine which owns this renderer.
        Engine* _engine;

        // A pointer to the back end.
        IRendererBackend* _backend;
    };
}