#pragma once

#include <string>

#include "../Events/Event.h"
#include "AssetData.h"

namespace Epoch {

    struct AssetLoadedEvent : public Event {
    public:
        const AssetData* Data;

    public:
        AssetLoadedEvent( const AssetData* assetData, void* sender ) : Event( EventType::ASSET_LOADED, sender ) {
            Data = assetData;
        }

        ~AssetLoadedEvent() {
            if( Data ) {
                delete Data;
                Data = nullptr;
            }
        }
    };
}