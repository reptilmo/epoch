
#include "EntityComponents/EntityComponent.h"
#include "Level.h"
#include "Entity.h"

namespace Epoch {

    Entity* Entity::Create( const TString& name ) {
        Entity* result = static_cast<Entity*>( WObject::Allocate( sizeof( Entity ) ) );

        // Use placement new to call constructor manually.
        new ( result )Entity();
        result->Name = name;
        return result;
    }

    void Entity::Destroy( Entity* entity ) {
        entity->Destroy();

        // Manually call destructor.
        entity->~Entity();
        WObject::Free( entity );
    }

    void Entity::Destroy() {
        if( _isDestroyed ) {
            return;
        }

        // Destroy children.
        U32 childCount = ChildCount();
        for( U32 i = 0; i < childCount; ++i ) {
            _children[i]->Destroy();
        }

        _isDestroyed = true;
    }

    void Entity::AddChild( Entity* child ) {
        _children.Add( child );
        child->setParent( this );
        child->setLevel( _level );
        _level->OnEntityAdded( child );
    }

    void Entity::AddChildAt( Entity* child, const U32 index ) {
        _children.InsertAt( child, index );
        child->setParent( this );
        child->setLevel( _level );
        _level->OnEntityAdded( child );
    }

    const bool Entity::RemoveChild( Entity* child ) {
        I32 index = _children.Remove( child );
        child->setParent( nullptr );
        child->setLevel( nullptr );
        _level->OnEntityRemoved( child );
        return index != -1;
    }

    Entity* Entity::RemoveChildAt( const U32 index ) {
        Entity* child = _children.Get( index );
        if( child ) {
            child->setParent( nullptr );
            _children.RemoveAt( index );
            child->setLevel( nullptr );
            _level->OnEntityRemoved( child );
        }
        return child;
    }

    void Entity::AddComponent( EntityComponent* component ) {
        _components.Add( component );
        component->setOwningEntity( this );
        if( component->IsRenderable() ) {
            _level->OnRenderableEntityComponentAdded( static_cast<RenderableEntityComponent*>( component ) );
        }
    }

    void Entity::AddComponentAt( EntityComponent* component, const U32 index ) {
        _components.InsertAt( component, index );
        component->setOwningEntity( this );
        if( component->IsRenderable() ) {
            _level->OnRenderableEntityComponentAdded( static_cast<RenderableEntityComponent*>( component ) );
        }
    }

    const bool Entity::RemoveComponent( EntityComponent* component ) {
        I32 index = _components.Remove( component );
        component->setOwningEntity( nullptr );
        if( component->IsRenderable() ) {
            _level->OnRenderableEntityComponentRemoved( static_cast<RenderableEntityComponent*>( component ) );
        }
        return index != -1;
    }

    EntityComponent* Entity::RemoveComponentAt( const U32 index ) {
        EntityComponent* component = _components.Get( index );
        if( component ) {
            component->setOwningEntity( nullptr );
            _components.RemoveAt( index );
            if( component->IsRenderable() ) {
                _level->OnRenderableEntityComponentRemoved( static_cast<RenderableEntityComponent*>( component ) );
            }
        }
        return component;
    }

    const Matrix4x4* Entity::GetWorldMatrix() {
        if( _worldMatrixDirty ) {
            if( _parent ) {

                // Iterate up the chain of matrices to calculate the world matrix.
                _worldMatrix = *( _parent->GetWorldMatrix() ) * _transform.GetTransformation();
            } else {
                _worldMatrix = _transform.GetTransformation();
            }
        }

        return &_worldMatrix;
    }

    void Entity::flagWorldMatrixDirty() {
        _worldMatrixDirty = true;
        U32 childCount = ChildCount();
        for( U32 i = 0; i < childCount; ++i ) {
            _children[i]->flagWorldMatrixDirty();
        }
    }
}