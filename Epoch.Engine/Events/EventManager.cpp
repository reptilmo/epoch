
#include <map>
#include <vector>

#include "../Logger.h"

#include "EventManager.h"

namespace Epoch {

    // Private, static map of events:handlers.
    std::map<const EventType, std::vector<IEventHandler*>*> _entries;

    void EventManager::Post( const Event event, const bool immediate ) {
        auto entry = _entries.find( event.Type );
        if( entry == _entries.end() ) {
            Logger::Trace( "EventManager::Post called for event type with no handlers listening." );
            return;
        } else {
            if( immediate ) {
                for( auto handler : *( ( *entry ).second ) ) {
                    handler->OnEvent( event );
                }
            } else {

                // TODO: Support queued events, which happen on the next frame.
                Logger::Fatal( "Non-immediate events not yet supported!" );
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
}