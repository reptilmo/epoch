#pragma once

#include "../Types.h"
#include "../World/EntityComponents/EntityComponent.h"

namespace Epoch {

    struct RenderReferenceData {
    public:
    public:

        RenderReferenceData(  RenderableComponentType componentType ) {
            _componentType = componentType;
        }

        RenderReferenceData( const RenderReferenceData& other ) {
            _componentType = other._componentType;
        }

        ~RenderReferenceData() {
            _componentType = RenderableComponentType::UNKNOWN;
        }

        const RenderableComponentType GetRenderableComponentType() const { return _componentType; }

    private:
        RenderableComponentType _componentType;
    };

}