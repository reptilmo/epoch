#pragma once

#include "../Types.h"

namespace Epoch {

    class IEventHandler;

    enum class EventType : U64 {
        UNKNOWN,
        WINDOW_RESIZED,
        ASSET_LOADED,
        KEY_DOWN,
        KEY_UP,

        EVENT_TYPE_MAX_EVENT_ID
    };

    /**
     * The base-level event structure to be used with the event system.
     */
    struct Event {

        /**
         * This event's type.
         */
        EventType Type;

        /**
         * The object which emitted this event.
         */
        void* Sender;

        /**
         * Default constructor.
         * 
         * @param type The type of this event.
         * @param sender The object which emitted this event.
         */
        Event( EventType type, void* sender );

        /**
         * Posts this event.
         * 
         * @param immediate Indicates if this event will be handled immediately or be queued on a subsequent frame. Default: false
         */
        void Post( const bool immediate );

        /**
         * Registers the given handler to recieve events of the given type.
         * 
         * @param type The type of event to listen for.
         * @param handler The event handler to register.
         */
        static void Listen( const EventType type, IEventHandler* handler );

        static void StopListening( const EventType type, IEventHandler* handler );
    };
}