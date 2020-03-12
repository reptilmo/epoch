#pragma once

#include "../../Types.h"

namespace Epoch {

    /**
     * Represents the backend of the renderer, which is an abstraction of the
     * underlying graphics API (such as Vulkan, DirectX or OpenGL)
     */
    class IRendererBackend {
    public:

        virtual ~IRendererBackend() {}

        /**
         * Initializes this renderer.
         * 
         * @returns True if successful; otherwise false.
         */
        virtual const bool Initialize() = 0;

        /**
         * Processes a single frame.
         * 
         * @param deltaTime The amount of time in seconds since the last frame.
         * 
         * @returns True on success, false on failure. Returning false crashes the application.
         */
        virtual const bool Frame( const F32 deltaTime ) = 0;
    };
}