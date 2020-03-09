#pragma once


#include "Event.h"
#include "IEventHandler.h"

namespace Epoch {

    class EventManager {
    public:
        void static Post( const Event event, const bool immediate );
        void static Listen( const EventType type, IEventHandler* handler );
        void static StopListening( const EventType type, IEventHandler* handler );

    private:
        // Private to enforce singleton pattern.
        EventManager() {}
        ~EventManager() {}
    };
}