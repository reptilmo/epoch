
#include <map>
#include <vector>
#include <queue>

#include "../Logger.h"

#include "EventManager.h"

namespace Epoch {

    // Private event queue
    std::queue<Event> _eventQueue;

    // Private, static map of events:handlers.
    std::map<const EventType, std::vector<IEventHandler*>*> _entries;

    void EventManager::Post( const Event event, const bool immediate ) {
        auto entry = _entries.find( event.Type );
        if( entry == _entries.end() ) {
            Logger::Trace( "EventManager::Post called for event type with no handlers listening." );
            return;
        } else {
            if( immediate ) {

                // TODO: Should probably be using an object pool for this instead of new/delete.
                processEvent( event );
            } else {
                _eventQueue.push( event );
            }
        }
    }

    void EventManager::Listen( const EventType type, IEventHandler* handler ) {
        auto entry = _entries.find( type );
        std::vector<IEventHandler*>* handlers = new std::vector<IEventHandler*>();
        if( entry == _entries.end() ) {
            auto it2 = _entries.emplace( type, handlers );

        } else {
            handlers = ( *entry ).second;
        }

        std::vector<IEventHandler*>::iterator it = std::find( handlers->begin(), handlers->end(), handler );
        if( it != handlers->end() ) {
            Logger::Warn( "EventManager::Listen called with already-listened-for handler. Handler not added." );
            return;
        }

        handlers->push_back( handler );
    }

    void EventManager::StopListening( const EventType type, IEventHandler* handler ) {
        auto entry = _entries.find( type );
        std::vector<IEventHandler*> handlers;
        if( entry == _entries.end() ) {
            Logger::Warn( "EventManager::StopListening called for an event whose type has no handlers. Nothing was done." );
        } else {
            handlers = ( *( *entry ).second );
        }

        std::vector<IEventHandler*>::iterator it = std::find( handlers.begin(), handlers.end(), handler );
        if( it == handlers.end() ) {
            Logger::Warn( "EventManager::StopListening called with non-listened-for handler. Nothing was done." );
            return;
        }

        handlers.erase( it );
    }

    void EventManager::Update( const F32 deltaTime ) {

        U32 processed = 0;
        while( !_eventQueue.empty() ) {
            Event e = _eventQueue.front();
            _eventQueue.pop();
            processEvent( e );

            processed++;

            // TODO: make this configurable.
            if( processed > 50 ) {
                Logger::Trace( "Too many messages to process on a single frame, deferring to next frame." );
                break;
            }
        }
    }

    void EventManager::processEvent( const Event& event ) {
        auto entry = _entries.find( event.Type );
        for( auto handler : *( ( *entry ).second ) ) {
            handler->OnEvent( &event );
        }
    }
}