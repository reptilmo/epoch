#pragma once

#include "../Types.h"

namespace Epoch {

    class IEventHandler;

    enum class EventType : U64 {
        UNKNOWN,
        WINDOW_RESIZED,

        EVENT_TYPE_MAX_EVENT_ID
    };

    struct Event {
        EventType Type;
        void* Sender;

        Event( EventType type, void* sender );

        /**
         * Posts this event.
         * 
         * @param immediate Indicates if this event will be handled immediately or be queued on a subsequent frame.
         */
        void Post( const bool immediate = true );

        /**
         * Registers the given handler to recieve events of the given type.
         * 
         * @param type The type of event to listen for.
         * @param handler The event handler to register.
         */
        static void Listen( const EventType type, IEventHandler* handler );
    };
}