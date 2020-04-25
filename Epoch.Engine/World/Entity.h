#pragma once

#include "../Types.h"
#include "../Defines.h"
#include "../String/TString.h"
#include "../Math/Transform.h"
#include "../Containers/List.h"
#include "WObject.h"

namespace Epoch {

    class Level;
    class EntityComponent;

    class EPOCH_API Entity : public WObject {
    public:
        TString Name;
    public:
        static Entity* Create( const TString& name );
        static void Destroy( Entity* entity );
    public:


        void Destroy();

        void AddChild( Entity* child );
        void AddChildAt( Entity* child, const U32 index );

        const bool RemoveChild( Entity* child );

        /*
         Removes the child entity at the given index. If no child is removed, nullptr is returned.
        */
        Entity* RemoveChildAt( const U32 index );

        void AddComponent( EntityComponent* component );
        void AddComponentAt( EntityComponent* component, const U32 index );

        const bool RemoveComponent( EntityComponent* component );

        /*
         Removes the child entity at the given index. If no child is removed, nullptr is returned.
        */
        EntityComponent* RemoveComponentAt( const U32 index );

        /**
         * Returns a const reference of this entity's world matrix. This should be used for rendering, for example.
         */
        const Matrix4x4* GetWorldMatrix();

        /**
         * Returns the number of children within this entity.
         */
        const U32 ChildCount() const { return _children.Size(); }

        /**
         * Returns the number of components within this entity.
         */
        const U32 ComponentCount() const { return _components.Size(); }

        /**
         * Indicates if this entity has been destroyed.
         */
        const bool IsDestroyed() const { return _isDestroyed; }

        /**
         * Returns a const pointer to the parent of this entity. If none, nullptr is returned.
         */
        const Entity* GetParent() const { return _parent; }

        /**
         * Returns a pointer to the parent of this entity. If none, nullptr is returned.
         */
        Entity* GetParent() { return _parent; }

        /**
         * Returns a const reference to the position of this entity.
         */
        const Vector3& GetPosition() const { return _transform.Position; }

        /**
         * Sets the position of this entity.
         */
        void SetPosition( const Vector3& value ) {
            _transform.Position = value;
            _worldMatrixDirty = true;
        }

        /**
         * Returns a const reference to the rotation of this entity.
         */
        const Quaternion& GetRotation() const { return _transform.Rotation; }

        /**
         * Sets the rotation of this entity.
         */
        void SetRotation( const Quaternion& value ) {
            _transform.Rotation = value;
            _worldMatrixDirty = true;
        }

        /**
         * Sets the position and rotation of this entity.
         *
         * @param position The position to be set.
         * @param rotation The rotation to be set.
         */
        void SetPositionAndRotation( const Vector3& position, const Quaternion& rotation ) {
            _transform.Position = position;
            _transform.Rotation = rotation;
            _worldMatrixDirty = true;
        }

        /**
         * Returns a const reference to the scale of this entity.
         */
        const Vector3& GetScale() const { return _transform.Scale; }

        /**
         * Sets the scale of this entity.
         */
        void SetScale( const Vector3& value ) {
            _transform.Scale = value;
            _worldMatrixDirty = true;
        }

        /**
         * Sets the position, rotation and scale of this entity.
         *
         * @param position The position to be set.
         * @param rotation The rotation to be set.
         * @param scale The scale to be set.
         */
        void SetPositionRotationAndScale( const Vector3& position, const Quaternion& rotation, const Vector3& scale ) {
            _transform.Position = position;
            _transform.Rotation = rotation;
            _transform.Scale = scale;
            _worldMatrixDirty = true;
        }

    protected:

        Entity() {}
        ~Entity() {}

        // Should be called whenever the transform is called. Recurses to all children.
        void flagWorldMatrixDirty();

        void setParent( Entity* parent ) { _parent = parent; }
        void setLevel( Level* level ) { _level = level; }

    protected:
        bool _isDestroyed = false;
        Entity* _parent = nullptr;
        Level* _level = nullptr;
        List<Entity*> _children;
        List<EntityComponent*> _components;

    private:
        bool _worldMatrixDirty = true;

        // Transform should never be changed directly. This is because any change should flag the world matrix as being dirty.
        Transform _transform;
        Matrix4x4 _worldMatrix;

        friend class Level;
    };
}