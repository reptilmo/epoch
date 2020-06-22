#pragma once


#include "Event.h"
#include "IEventHandler.h"

namespace Epoch {

    /*
     Manages events across the system. A new event may be posted, to which listeners will
     listen. Events have a type, which may be 
    */
    class EventManager {
    public:
        static void Post( const Event event, const bool immediate );
        static void Listen( const EventType type, IEventHandler* handler );
        static void StopListening( const EventType type, IEventHandler* handler );

        static void Update( const F32 deltaTime );

    private:
        static void processEvent( const Event& event );

    private:
        // Private to enforce singleton pattern.
        EventManager() {}
        ~EventManager() {}
    };
}