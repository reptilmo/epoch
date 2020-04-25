
#include "../Entity.h"
#include "EntityComponent.h"

namespace Epoch {

    Entity* EntityComponent::getOwningEntity() {
        return _owner;
    }

    void EntityComponent::setOwningEntity( Entity* entity ) {
        _owner = entity;
    }

    const Matrix4x4* RenderableEntityComponent::GetWorldMatrix() { 
        return getOwningEntity()->GetWorldMatrix(); 
    }
}