
#include "EventManager.h"
#include "Event.h"

namespace Epoch {

    Event::Event( EventType type, void* sender ) {
        Type = type;
        Sender = sender;
    }

    void Event::Post( const bool immediate ) {
        EventManager::Post( *this, immediate );
    }

    void Event::Listen( const EventType type, IEventHandler* handler ) {
        EventManager::Listen( type, handler );
    }

    void Event::StopListening( const EventType type, IEventHandler* handler ) {
        EventManager::StopListening( type, handler );
    }
}