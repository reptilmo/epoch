#pragma once

namespace Epoch {

    struct Event;

    /**
     * Represents an object which can process events.
     */
    class IEventHandler {
    public:

        /**
         * Processes the given event.
         * 
         * @param event The event to be processed.
         */
        virtual void OnEvent( const Event* event ) = 0;
    };
}