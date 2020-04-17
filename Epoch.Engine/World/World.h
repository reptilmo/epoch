#pragma once

#include "../Types.h"
#include "../String/TString.h"
#include "../Math/Transform.h"
#include "../Containers/List.h"

namespace Epoch {

    /**
     * Represents the base for all objects which exist in the world. Every object is assigned a unique
     * numeric identifier which can later be used for retrieval of an instance by that identifier.
     */
    class WObject {
    public:
        WObject();
        ~WObject();

        const U32 GetId() const { return _id; }

    private:
        static U32 GLOBAL_OBJECT_ID;
    private:
        U32 _id;
    };

    class IEntityComponent {
    public:

    };

    class Entity : public WObject {
    public:
        TString Name;
    public:
        Entity();
        Entity( const TString& name );
        ~Entity();

        void AddChild( Entity* child );
        void AddChildAt( Entity* child, const U32 index );

        void RemoveChild( Entity* child );
        void RemoveChildAt( Entity* child );

        const Entity const* GetParent() const { return _parent; }
        Entity* GetParent() { return _parent; }

        Transform* GetTransform() { return &_transform; }
        const Transform* GetTransform() const { return &_transform; }

    protected:
        void setParent( Entity* parent ) { _parent = parent; }

    protected:
        Entity* _parent;
        Transform _transform;
        List<Entity*> _children;
    };

    class World {
    public:
        World();
        ~World();
    };
}
